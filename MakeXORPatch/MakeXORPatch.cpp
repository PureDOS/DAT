//--------------------------------------------//
// MakeXORPatch                               //
// License: Public Domain (www.unlicense.org) //
//--------------------------------------------//

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#if defined(_MSC_VER)
#include <sys/utime.h>
typedef unsigned __int64 Bit64u;
#else
#include <utime.h>
typedef unsigned long long Bit64u;
#endif

#ifdef WIN32
#define CROSS_FILESPLIT '\\'
#else
#define CROSS_FILESPLIT '/'
#endif

// Use 64-bit fseek and ftell
#if defined(_MSC_VER) && _MSC_VER >= 1400 // VC2005 and up have a special 64-bit fseek
#define fseek_wrap(fp, offset, whence) _fseeki64(fp, (__int64)offset, whence)
#define ftell_wrap(fp) _ftelli64(fp)
#elif defined(HAVE_64BIT_OFFSETS) || (defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE - 0) >= 200112) || (defined(__POSIX_VISIBLE) && __POSIX_VISIBLE >= 200112) || (defined(_POSIX_VERSION) && _POSIX_VERSION >= 200112) || __USE_LARGEFILE || (defined(_FILE_OFFSET_BITS) && _FILE_OFFSET_BITS == 64)
#define fseek_wrap(fp, offset, whence) fseeko(fp, (off_t)offset, whence)
#define ftell_wrap(fp) ftello(fp)
#else
#define fseek_wrap(fp, offset, whence) fseek(fp, (long)offset, whence)
#define ftell_wrap(fp) ftell(fp)
#endif

#define XOR_WRITE_LE32(p,v) { ((unsigned char*)(p))[0] = (unsigned char)((unsigned)(v) & 0xFF); ((unsigned char*)(p))[1] = (unsigned char)(((unsigned)(v) >> 8) & 0xFF); ((unsigned char*)(p))[2] = (unsigned char)(((unsigned)(v) >> 16) & 0xFF); ((unsigned char*)(p))[3] = (unsigned char)((unsigned)(v) >> 24); }
#define XOR_READ_LE32(p) ((unsigned)(((const unsigned char *)(p))[0]) | ((unsigned)(((const unsigned char *)(p))[1]) << 8U) | ((unsigned)(((const unsigned char *)(p))[2]) << 16U) | ((unsigned)(((const unsigned char *)(p))[3]) << 24U))

static void XORAndClose(FILE* fBase, FILE* fXORPair, FILE* fOutput, const char* targetPath, const char* outputPath, Bit64u targetSize, bool writeFinalZeroes)
{
	Bit64u zeroes = 0;
	for (unsigned char byteBase = 0, byteTarget; targetSize; targetSize--)
	{
		if (!fread(&byteTarget, 1, 1, fXORPair)) byteTarget = 0;
		if (!fread(&byteBase, 1, 1, fBase)) { fseek_wrap(fXORPair, 0, SEEK_SET); fread(&byteBase, 1, 1, fBase); }
		unsigned char byteOutput = byteBase ^ byteTarget;
		if (!byteOutput) { zeroes++; continue; }
		if (zeroes) { for (Bit64u nulls = 0, n; (n = (zeroes > 7 ? 8 : zeroes)) != 0; zeroes -= n) fwrite(&nulls, 1, (int)n, fOutput); }
		fwrite(&byteOutput, 1, 1, fOutput);
	}
	if (zeroes && writeFinalZeroes) { for (Bit64u nulls = 0, n; (n = (zeroes > 7 ? 8 : zeroes)) != 0; zeroes -= n) fwrite(&nulls, 1, (int)n, fOutput); }
	fclose(fOutput);

	// Copy modification time stamp from target to newly output file
	struct stat targetStat;
	struct utimbuf new_times;
	stat(targetPath, &targetStat);
	new_times.actime = targetStat.st_atime;
	new_times.modtime = targetStat.st_mtime;
	utime(outputPath, &new_times);
}

int main(int argc, char *argv[])
{
	if (argc == 4) // Create XOR Patch
	{
		const char *basePath = argv[1], *targetPath = argv[2], *outputPath = argv[3], *relPath = basePath;
		if ((basePath[0] == '/' && targetPath[0] != '/') || (basePath[0] == '\\' && targetPath[0] != '\\') || (basePath[1] == ':' && targetPath[1] != ':'))
		{
			fprintf(stderr, "Both BASE and TARGET paths must be absolute or relative.\n\nRun tool with:\n  %s <BASE FILE> <TARGET FILE> <OUTPUT .XOR PATCH FILE>\n\n", (argc ? argv[0] : "MakeXORPatch"));
			return 1;
		}
		if (!strcmp(basePath, targetPath)) { fprintf(stderr, "Base file '%s' can't be same as target file\n\n", basePath); return 1; }
		if (fopen(outputPath, "rb")) { fprintf(stderr, "Output file '%s' already exists\n\n", outputPath); return 1; }
		FILE* fBase   = fopen(basePath, "rb");   if (!fBase)   { fprintf(stderr, "Unable to open base file '%s'\n\n",   basePath);   return 1; }
		FILE* fTarget = fopen(targetPath, "rb"); if (!fTarget) { fprintf(stderr, "Unable to open target file '%s'\n\n", targetPath); return 1; }
		FILE* fOutput = fopen(outputPath, "wb"); if (!fOutput) { fprintf(stderr, "Unable to open output file '%s'\n\n", outputPath); return 1; }
		fseek_wrap(fBase,   0, SEEK_END); fseek_wrap(fTarget, 0, SEEK_END); Bit64u targetSize = (Bit64u)ftell_wrap(fTarget);
		if (!ftell_wrap(fBase)) { fprintf(stderr, "Base file '%s' is an empty file and cannot be used\n\n",   basePath);   return 1; } fseek_wrap(fBase,   0, SEEK_SET);
		if (!targetSize)        { fprintf(stderr, "Target file '%s' is an empty file and cannot be used\n\n", targetPath); return 1; } fseek_wrap(fTarget, 0, SEEK_SET);

		int relativeParentDirs = 0;
		for (int lastSlash = 0, i = 0;; i++)
		{
			if (basePath[i] == targetPath[i])
			{
				if (basePath[i] == '\\' || basePath[i] == '/') lastSlash = i;
			}
			else
			{
				if (lastSlash) relPath = basePath + lastSlash + 1;
				for (; targetPath[i]; i++) { if (targetPath[i] == '\\' || targetPath[i] == '/') relativeParentDirs++; }
				break;
			}
		}

		printf("Generating XOR Patch '%s' with reference to base file '", outputPath);
		for (int j = 0; j != relativeParentDirs; j++) printf("../");
		for (const char* k = relPath; *k; k++) printf("%c", (*k == '\\' ? '/' : *k));
		printf("' ...\n");

		unsigned char hdrMagic[4], baseOffset = 0;
		XOR_WRITE_LE32(hdrMagic, 0x50524F58);
		fwrite(hdrMagic, sizeof(hdrMagic), 1, fOutput);

		for (int l = 0; l != relativeParentDirs; l++) fwrite("../", 3, 1, fOutput);
		for (const char* m = relPath; ; m++) { char c = (*m == '\\' ? '/' : *m); fwrite(&c, 1, 1, fOutput); if (!c) break; }
		fwrite(&baseOffset, 1, 1, fOutput); // offset to target encoded as LEB128 (for now fixed to 0)
		for (Bit64u n = targetSize;;) { unsigned char b = (unsigned char)((n & 0x7F) | ((n > 0x7F) ? 0x80 : 0)); fwrite(&b, 1, 1, fOutput); if (!(n >>= 7)) break; } // target size encoded as LEB128

		XORAndClose(fBase, fTarget, fOutput, targetPath, outputPath, targetSize, false);
	}
	else if (argc == 2 || argc == 3)
	{
		const char *xorPath = argv[1];
		FILE* fXOR = fopen(xorPath, "rb"); if (!fXOR) { fprintf(stderr, "Unable to open XOR file '%s'\n\n", xorPath); return 1; }
		unsigned char hdrMagic[4];
		if (!fread(hdrMagic, sizeof(hdrMagic), 1, fXOR) || XOR_READ_LE32(hdrMagic) != 0x50524F58)  { fprintf(stderr, "XOR file '%s' is not an XOR patch file\n\n", xorPath); return 1; }
		char basePath[512];
		unsigned basePathLen = 0;
		for (char c; fread(&c, 1, 1, fXOR) && c; basePathLen++) basePath[basePathLen] = ((c == '\\' || c == '/') ? CROSS_FILESPLIT : c);
		basePath[basePathLen] = '\0';
		Bit64u baseOffset = 0, targetSize = 0;
		for (unsigned char b1, num1 = 0; fread(&b1, 1, 1, fXOR) && b1;) baseOffset |= (Bit64u)b1 << (7 * (num1++));
		for (unsigned char b2, num2 = 0; fread(&b2, 1, 1, fXOR) && b2;) targetSize |= (Bit64u)b2 << (7 * (num2++));

		if (argc == 3) // Apply XOR Patch
		{
			const char *outputPath = argv[2];
			if (fopen(outputPath, "rb")) { fprintf(stderr, "Output file '%s' already exists\n\n", outputPath); return 1; }
			FILE* fBase   = fopen(basePath,   "rb"); if (!fBase)   { fprintf(stderr, "Unable to open referenced base file '%s'\n\n", basePath);   return 1; }
			FILE* fOutput = fopen(outputPath, "wb"); if (!fOutput) { fprintf(stderr, "Unable to open output file '%s'\n\n",          outputPath); return 1; }
			fseek_wrap(fBase, baseOffset, SEEK_SET);
			XORAndClose(fBase, fXOR, fOutput, xorPath, outputPath, targetSize, true);
		}
		else if (argc == 2) // Get XOR Patch info
		{
			printf("XOR Patch File:       %s\n", xorPath);
			printf("Referenced Base File: %s\n", basePath);
			printf("Base File Offset:     %u\n", (unsigned)baseOffset);
			printf("Default Target Name:  %.*s\n", (strlen(xorPath) > 4 ? (int)(strlen(xorPath)-4) : 0), xorPath);
			printf("Target Result Size:   %u\n", (unsigned)targetSize);
			struct stat xorStat;
			stat(xorPath, &xorStat);
			struct tm xorMTime = *localtime(&xorStat.st_mtime);
			printf("Target Time Stamp:    %04d-%02d-%02d %02d:%02d:%02d\n", xorMTime.tm_year + 1900, xorMTime.tm_mon + 1, xorMTime.tm_mday, xorMTime.tm_hour, xorMTime.tm_min, xorMTime.tm_sec);
			printf("\n");
		}
	}
	else
	{
		fprintf(stderr, "Wrong arguments.\n\n");
		fprintf(stderr, "  Create XOR Patch:   %s <BASE FILE> <TARGET FILE> <OUTPUT .XOR PATCH FILE>\n\n", (argc ? argv[0] : "MakeXORPatch"));
		fprintf(stderr, "  Apply XOR Patch:    %s <.XOR PATCH FILE> <OUTPUT TARGET FILE>\n\n", (argc ? argv[0] : "MakeXORPatch"));
		fprintf(stderr, "  Get XOR Patch info: %s <.XOR PATCH FILE>\n\n", (argc ? argv[0] : "MakeXORPatch"));
		return 1;
	}
	return 0;
}
