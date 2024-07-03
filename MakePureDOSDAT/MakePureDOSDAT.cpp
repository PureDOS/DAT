//--------------------------------------------//
// MakePureDOSDAT                             //
// License: Public Domain (www.unlicense.org) //
//--------------------------------------------//

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <vector>

typedef unsigned char Bit8u;
typedef unsigned short Bit16u;
typedef signed short Bit16s;
typedef unsigned int Bit32u;
typedef signed int Bit32s;
#if defined(_MSC_VER)
typedef unsigned __int64 Bit64u;
#else
typedef unsigned long long Bit64u;
#endif

#if defined (_MSC_VER)
#define strcasecmp(a,b) stricmp(a,b)
#define strncasecmp(a,b,n) _strnicmp(a,b,n)
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

static Bit32u CRC32(const void* data, size_t data_size)
{
	// A compact CCITT crc16 and crc32 C implementation that balances processor cache usage against speed
	// By Karl Malbrain - http://www.geocities.ws/malbrain/
	static const Bit32u s_crc32[16] = { 0, 0x1db71064, 0x3b6e20c8, 0x26d930ac, 0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c, 0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c, 0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c };
	Bit32u crcu32 = (Bit32u)~(Bit32u)0;
	for (Bit8u b, *p = (Bit8u*)data;data_size--;) { b = *p++; crcu32 = (crcu32 >> 4) ^ s_crc32[(crcu32 & 0xF) ^ (b & 0xF)]; crcu32 = (crcu32 >> 4) ^ s_crc32[(crcu32 & 0xF) ^ (b >> 4)]; }
	return ~crcu32;
}

#if 0
static void MD5(const void* data, size_t data_size, Bit8u res[16])
{
	// BASED ON MD5 (public domain)
	// By Bryce Wilson - https://github.com/Zunawe/md5-c
	static const Bit8u PADDING[64] = { 0x80, 0 };
	struct MD5_CTX
	{
		Bit32u a, b, c, d;
		void Step(const Bit32u* block)
		{
			static const Bit32u S[64] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };
			static const Bit32u K[64] = {0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8, 0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, 0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };
			Bit32u AA = a, BB = b, CC = c, DD = d, E;
			for (Bit32u i = 0, j; i != 64; i++)
			{
				switch (i / 16)
				{
					case 0: E = ((BB & CC) | (~BB & DD)); j = i; break;
					case 1: E = ((BB & DD) | (CC & ~DD)); j = ((i * 5) + 1) % 16; break;
					case 2: E = (BB ^ CC ^ DD); j = ((i * 3) + 5) % 16; break;
					default: E = (CC ^ (BB | ~DD)); j = (i * 7) % 16; break;
				}
				Bit32u temp = DD, x = AA + E + K[i] + block[j], n = S[i];
				DD = CC; CC = BB; BB = BB + ((x << n) | (x >> (32 - n))); AA = temp;
			}
			a += AA; b += BB; c += CC; d += DD;
		}
		void Update(Bit8u* tmp, size_t offset, const Bit8u* data, size_t len)
		{
			for (const Bit8u* end = data + len; data != end; data++)
			{
				tmp[offset++] = *data;
				if (!(offset % 64 == 0)) continue;
				Bit32u block[16];
				for (Bit32u j = 0; j != 16; j++) block[j] = ((Bit32u)tmp[j*4+3]<<24) | ((Bit32u)tmp[j*4+2]<<16) | ((Bit32u)tmp[j*4+1]<<8) | (Bit32u)tmp[j*4];
				Step(block);
				offset = 0;
			}
		}
	} ctx = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476 };
	Bit8u tmp[64]; Bit32u last[16], offset = (data_size % 64);
	ctx.Update(tmp, 0, (const Bit8u *)data, data_size);
	ctx.Update(tmp, offset, PADDING, (offset < 56 ? 56 - offset : (56 + 64) - offset));
	for (Bit32u j = 0; j != 14; j++) last[j] = ((Bit32u)tmp[j*4+3]<<24) | ((Bit32u)tmp[j*4+2]<<16) | ((Bit32u)tmp[j*4+1]<<8) | (Bit32u)tmp[j*4];
	last[14] = (Bit32u)(data_size * 8); last[15] = (Bit32u)(((Bit64u)data_size * 8) >> 32);
	ctx.Step(last);
	for (Bit32u i = 0; i != 16; i++) res[i] = (Bit8u)((((Bit32u*)&ctx.a)[i/4] >> ((i%4)*8)) & 0x000000FF);
}
#endif

static void FastMD5(const void* data, size_t data_size, Bit8u res[16])
{
	// BASED ON MD5 (public domain)
	// By Galen Guyer - https://github.com/galenguyer/md5
	struct MD5_CTX
	{
		Bit32u A, B, C, D;
		const void* Body(const void *data, size_t size)
		{
			const Bit8u *ptr = (const Bit8u*)data;
			Bit32u a = A, b = B, c = C, d = D;
			do
			{
				Bit32u saved_a = a, saved_b = b, saved_c = c, saved_d = d;
				#define STEP(f, a, b, c, d, x, t, s) (a) += f((b), (c), (d)) + (x) + (t); (a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s)))); (a) += (b);
				#if defined(__i386__) || _M_IX86 || defined(__x86_64__) || _M_AMD64 || defined(__vax__)
				#define SET(n) (*(Bit32u *)&ptr[(n) * 4])
				#define GET(n) SET(n)
				#else
				Bit32u block[16];
				#define SET(n) (block[(n)] = (Bit32u)ptr[(n) * 4] | ((Bit32u)ptr[(n) * 4 + 1] << 8) | ((Bit32u)ptr[(n) * 4 + 2] << 16) | ((Bit32u)ptr[(n) * 4 + 3] << 24))
				#define GET(n) (block[(n)])
				#endif
				#define F(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
				#define G(x, y, z) ((y) ^ ((z) & ((x) ^ (y))))
				#define H(x, y, z) (((x) ^ (y)) ^ (z))
				#define J(x, y, z) ((x) ^ ((y) ^ (z)))
				#define I(x, y, z) ((y) ^ ((x) | ~(z)))
				STEP(F, a, b, c, d, SET( 0), 0xd76aa478,  7) STEP(F, d, a, b, c, SET( 1), 0xe8c7b756, 12) STEP(F, c, d, a, b, SET( 2), 0x242070db, 17) STEP(F, b, c, d, a, SET( 3), 0xc1bdceee, 22)
				STEP(F, a, b, c, d, SET( 4), 0xf57c0faf,  7) STEP(F, d, a, b, c, SET( 5), 0x4787c62a, 12) STEP(F, c, d, a, b, SET( 6), 0xa8304613, 17) STEP(F, b, c, d, a, SET( 7), 0xfd469501, 22)
				STEP(F, a, b, c, d, SET( 8), 0x698098d8,  7) STEP(F, d, a, b, c, SET( 9), 0x8b44f7af, 12) STEP(F, c, d, a, b, SET(10), 0xffff5bb1, 17) STEP(F, b, c, d, a, SET(11), 0x895cd7be, 22)
				STEP(F, a, b, c, d, SET(12), 0x6b901122,  7) STEP(F, d, a, b, c, SET(13), 0xfd987193, 12) STEP(F, c, d, a, b, SET(14), 0xa679438e, 17) STEP(F, b, c, d, a, SET(15), 0x49b40821, 22)
				STEP(G, a, b, c, d, GET( 1), 0xf61e2562,  5) STEP(G, d, a, b, c, GET( 6), 0xc040b340,  9) STEP(G, c, d, a, b, GET(11), 0x265e5a51, 14) STEP(G, b, c, d, a, GET( 0), 0xe9b6c7aa, 20)
				STEP(G, a, b, c, d, GET( 5), 0xd62f105d,  5) STEP(G, d, a, b, c, GET(10), 0x02441453,  9) STEP(G, c, d, a, b, GET(15), 0xd8a1e681, 14) STEP(G, b, c, d, a, GET( 4), 0xe7d3fbc8, 20)
				STEP(G, a, b, c, d, GET( 9), 0x21e1cde6,  5) STEP(G, d, a, b, c, GET(14), 0xc33707d6,  9) STEP(G, c, d, a, b, GET( 3), 0xf4d50d87, 14) STEP(G, b, c, d, a, GET( 8), 0x455a14ed, 20)
				STEP(G, a, b, c, d, GET(13), 0xa9e3e905,  5) STEP(G, d, a, b, c, GET( 2), 0xfcefa3f8,  9) STEP(G, c, d, a, b, GET( 7), 0x676f02d9, 14) STEP(G, b, c, d, a, GET(12), 0x8d2a4c8a, 20)
				STEP(H, a, b, c, d, GET( 5), 0xfffa3942,  4) STEP(J, d, a, b, c, GET( 8), 0x8771f681, 11) STEP(H, c, d, a, b, GET(11), 0x6d9d6122, 16) STEP(J, b, c, d, a, GET(14), 0xfde5380c, 23)
				STEP(H, a, b, c, d, GET( 1), 0xa4beea44,  4) STEP(J, d, a, b, c, GET( 4), 0x4bdecfa9, 11) STEP(H, c, d, a, b, GET( 7), 0xf6bb4b60, 16) STEP(J, b, c, d, a, GET(10), 0xbebfbc70, 23)
				STEP(H, a, b, c, d, GET(13), 0x289b7ec6,  4) STEP(J, d, a, b, c, GET( 0), 0xeaa127fa, 11) STEP(H, c, d, a, b, GET( 3), 0xd4ef3085, 16) STEP(J, b, c, d, a, GET( 6), 0x04881d05, 23)
				STEP(H, a, b, c, d, GET( 9), 0xd9d4d039,  4) STEP(J, d, a, b, c, GET(12), 0xe6db99e5, 11) STEP(H, c, d, a, b, GET(15), 0x1fa27cf8, 16) STEP(J, b, c, d, a, GET( 2), 0xc4ac5665, 23)
				STEP(I, a, b, c, d, GET( 0), 0xf4292244,  6) STEP(I, d, a, b, c, GET( 7), 0x432aff97, 10) STEP(I, c, d, a, b, GET(14), 0xab9423a7, 15) STEP(I, b, c, d, a, GET( 5), 0xfc93a039, 21)
				STEP(I, a, b, c, d, GET(12), 0x655b59c3,  6) STEP(I, d, a, b, c, GET( 3), 0x8f0ccc92, 10) STEP(I, c, d, a, b, GET(10), 0xffeff47d, 15) STEP(I, b, c, d, a, GET( 1), 0x85845dd1, 21)
				STEP(I, a, b, c, d, GET( 8), 0x6fa87e4f,  6) STEP(I, d, a, b, c, GET(15), 0xfe2ce6e0, 10) STEP(I, c, d, a, b, GET( 6), 0xa3014314, 15) STEP(I, b, c, d, a, GET(13), 0x4e0811a1, 21)
				STEP(I, a, b, c, d, GET( 4), 0xf7537e82,  6) STEP(I, d, a, b, c, GET(11), 0xbd3af235, 10) STEP(I, c, d, a, b, GET( 2), 0x2ad7d2bb, 15) STEP(I, b, c, d, a, GET( 9), 0xeb86d391, 21)
				#undef F
				#undef G
				#undef H
				#undef J
				#undef I
				#undef GET
				#undef SET
				#undef STEP
				a += saved_a; b += saved_b; c += saved_c; d += saved_d; ptr += 64;
			} while (size -= 64);
			A = a; B = b; C = c; D = d;
			return ptr;
		}
	} ctx = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476 };
	size_t ctx_lo = (data_size & 0x1fffffff) << 3, ctx_hi = data_size >> 29;
	if (data_size >= 64)
	{
		data = ctx.Body(data, data_size & ~(unsigned long)63);
		data_size &= 63;
	}
	Bit8u ctx_buffer[64];
	memcpy(ctx_buffer, data, data_size);
	ctx_buffer[data_size++] = 0x80;
	size_t available = 64 - data_size;
	if (available < 8)
	{
		memset(&ctx_buffer[data_size], 0, available);
		ctx.Body(ctx_buffer, 64);
		data_size = 0;
		available = 64;
	}
	memset(&ctx_buffer[data_size], 0, available - 8);
	#define OUT(dst, src) (dst)[0] = (Bit8u)(src); (dst)[1] = (Bit8u)((src) >> 8); (dst)[2] = (Bit8u)((src) >> 16); (dst)[3] = (Bit8u)((src) >> 24);
	OUT(&ctx_buffer[56], ctx_lo)
	OUT(&ctx_buffer[60], ctx_hi)
	ctx.Body(ctx_buffer, 64);
	OUT(&res[0], ctx.A)
	OUT(&res[4], ctx.B)
	OUT(&res[8], ctx.C)
	OUT(&res[12], ctx.D)
	#undef OUT
}

static void SHA1(const void* data, size_t data_size, Bit8u res[20])
{
	// BASED ON SHA-1 in C (public domain)
	// By Steve Reid - https://github.com/clibs/sha1
	struct SHA1_CTX
	{
		static void Transform(Bit32u* state, const void* buffer)
		{
			Bit32u block[16];
			memcpy(block, buffer, 64);
			Bit32u a = state[0], b = state[1], c = state[2], d = state[3], e = state[4];
			#define SHA1ROL(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))
			#define SHA1BLK0(i) (block[i] = (SHA1ROL(block[i],24)&0xFF00FF00)|(SHA1ROL(block[i],8)&0x00FF00FF))
			#define SHA1BLK(i) (block[i&15] = SHA1ROL(block[(i+13)&15]^block[(i+8)&15]^block[(i+2)&15]^block[i&15],1))
			#define SHA1R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+SHA1BLK0(i)+0x5A827999+SHA1ROL(v,5);w=SHA1ROL(w,30);
			#define SHA1R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+SHA1BLK(i)+0x5A827999+SHA1ROL(v,5);w=SHA1ROL(w,30);
			#define SHA1R2(v,w,x,y,z,i) z+=(w^x^y)+SHA1BLK(i)+0x6ED9EBA1+SHA1ROL(v,5);w=SHA1ROL(w,30);
			#define SHA1R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+SHA1BLK(i)+0x8F1BBCDC+SHA1ROL(v,5);w=SHA1ROL(w,30);
			#define SHA1R4(v,w,x,y,z,i) z+=(w^x^y)+SHA1BLK(i)+0xCA62C1D6+SHA1ROL(v,5);w=SHA1ROL(w,30);
			SHA1R0(a,b,c,d,e, 0); SHA1R0(e,a,b,c,d, 1); SHA1R0(d,e,a,b,c, 2); SHA1R0(c,d,e,a,b, 3);
			SHA1R0(b,c,d,e,a, 4); SHA1R0(a,b,c,d,e, 5); SHA1R0(e,a,b,c,d, 6); SHA1R0(d,e,a,b,c, 7);
			SHA1R0(c,d,e,a,b, 8); SHA1R0(b,c,d,e,a, 9); SHA1R0(a,b,c,d,e,10); SHA1R0(e,a,b,c,d,11);
			SHA1R0(d,e,a,b,c,12); SHA1R0(c,d,e,a,b,13); SHA1R0(b,c,d,e,a,14); SHA1R0(a,b,c,d,e,15);
			SHA1R1(e,a,b,c,d,16); SHA1R1(d,e,a,b,c,17); SHA1R1(c,d,e,a,b,18); SHA1R1(b,c,d,e,a,19);
			SHA1R2(a,b,c,d,e,20); SHA1R2(e,a,b,c,d,21); SHA1R2(d,e,a,b,c,22); SHA1R2(c,d,e,a,b,23);
			SHA1R2(b,c,d,e,a,24); SHA1R2(a,b,c,d,e,25); SHA1R2(e,a,b,c,d,26); SHA1R2(d,e,a,b,c,27);
			SHA1R2(c,d,e,a,b,28); SHA1R2(b,c,d,e,a,29); SHA1R2(a,b,c,d,e,30); SHA1R2(e,a,b,c,d,31);
			SHA1R2(d,e,a,b,c,32); SHA1R2(c,d,e,a,b,33); SHA1R2(b,c,d,e,a,34); SHA1R2(a,b,c,d,e,35);
			SHA1R2(e,a,b,c,d,36); SHA1R2(d,e,a,b,c,37); SHA1R2(c,d,e,a,b,38); SHA1R2(b,c,d,e,a,39);
			SHA1R3(a,b,c,d,e,40); SHA1R3(e,a,b,c,d,41); SHA1R3(d,e,a,b,c,42); SHA1R3(c,d,e,a,b,43);
			SHA1R3(b,c,d,e,a,44); SHA1R3(a,b,c,d,e,45); SHA1R3(e,a,b,c,d,46); SHA1R3(d,e,a,b,c,47);
			SHA1R3(c,d,e,a,b,48); SHA1R3(b,c,d,e,a,49); SHA1R3(a,b,c,d,e,50); SHA1R3(e,a,b,c,d,51);
			SHA1R3(d,e,a,b,c,52); SHA1R3(c,d,e,a,b,53); SHA1R3(b,c,d,e,a,54); SHA1R3(a,b,c,d,e,55);
			SHA1R3(e,a,b,c,d,56); SHA1R3(d,e,a,b,c,57); SHA1R3(c,d,e,a,b,58); SHA1R3(b,c,d,e,a,59);
			SHA1R4(a,b,c,d,e,60); SHA1R4(e,a,b,c,d,61); SHA1R4(d,e,a,b,c,62); SHA1R4(c,d,e,a,b,63);
			SHA1R4(b,c,d,e,a,64); SHA1R4(a,b,c,d,e,65); SHA1R4(e,a,b,c,d,66); SHA1R4(d,e,a,b,c,67);
			SHA1R4(c,d,e,a,b,68); SHA1R4(b,c,d,e,a,69); SHA1R4(a,b,c,d,e,70); SHA1R4(e,a,b,c,d,71);
			SHA1R4(d,e,a,b,c,72); SHA1R4(c,d,e,a,b,73); SHA1R4(b,c,d,e,a,74); SHA1R4(a,b,c,d,e,75);
			SHA1R4(e,a,b,c,d,76); SHA1R4(d,e,a,b,c,77); SHA1R4(c,d,e,a,b,78); SHA1R4(b,c,d,e,a,79);
			state[0] += a; state[1] += b; state[2] += c; state[3] += d; state[4] += e;
		}
		void Process(const Bit8u* data, size_t len)
		{
			size_t i, j = count[0];
			if ((count[0] += (len << 3)) < j) count[1]++;
			count[1] += (len>>29);
			j = (j >> 3) & 63;
			if ((j + len) > 63)
			{
				memcpy(&buffer[j], data, (i = 64-j));
				Transform(state, buffer);
				for (; i + 63 < len; i += 64) Transform(state, &data[i]);
				j = 0;
			}
			else i = 0;
			memcpy(&buffer[j], &data[i], len - i);
		}
		size_t count[2];
		Bit32u state[5];
		Bit8u buffer[64];
	} ctx;
	ctx.count[0] = ctx.count[1] = 0;
	ctx.state[0] = 0x67452301;
	ctx.state[1] = 0xEFCDAB89;
	ctx.state[2] = 0x98BADCFE;
	ctx.state[3] = 0x10325476;
	ctx.state[4] = 0xC3D2E1F0;
	ctx.Process((const Bit8u*)data, data_size);
	Bit8u finalcount[8];
	for (unsigned i = 0; i < 8; i++)  finalcount[i] = (Bit8u)((ctx.count[(i >= 4 ? 0 : 1)] >> ((3-(i & 3)) * 8) ) & 255);
	Bit8u c = 0200;
	ctx.Process(&c, 1);
	while ((ctx.count[0] & 504) != 448) { c = 0000; ctx.Process(&c, 1); }
	ctx.Process(finalcount, 8);
	for (unsigned j = 0; j < 20; j++) res[j] = (Bit8u)((ctx.state[j>>2] >> ((3-(j & 3)) * 8) ) & 255);
}

struct Zip_Archive
{
	FILE* zip;
	Bit64u ofs, size;

	Zip_Archive(FILE* _zip) : zip(_zip)
	{
		fseek_wrap(zip, 0, SEEK_END);
		ofs = size = (Bit64u)ftell_wrap(zip);
	}

	~Zip_Archive()
	{
		if (!zip) return;
		fclose(zip);
	}

	Bit32u Read(Bit64u seek_ofs, void *pBuf, Bit32u n)
	{
		if (seek_ofs >= size) n = 0;
		else if ((Bit64u)n > (size - seek_ofs)) n = (Bit32u)(size - seek_ofs);
		if (seek_ofs != ofs) { fseek_wrap(zip, seek_ofs, SEEK_SET); ofs = seek_ofs; }
		Bit32u got = (Bit32u)fread(pBuf, 1, n, zip);
		ofs += got;
		return got;
	}

	bool Unpack(Bit64u zf_data_ofs, Bit32u zf_comp_size, Bit32u zf_uncomp_size, Bit8u zf_bit_flags, Bit8u zf_method, std::vector<Bit8u>& mem_data);
	enum { METHOD_STORED = 0, METHOD_SHRUNK = 1, METHOD_IMPLODED = 6, METHOD_DEFLATED = 8 };
	static bool MethodSupported(Bit32u method) { return (method == METHOD_DEFLATED || method == METHOD_STORED || method == METHOD_SHRUNK || method == METHOD_IMPLODED); }
};

// Various ZIP archive enums. To completely avoid cross platform compiler alignment and platform endian issues, we don't use structs for any of this stuff
enum
{
	// ZIP archive identifiers and record sizes
	ZIP_END_OF_CENTRAL_DIR_HEADER_SIG = 0x06054b50, ZIP_CENTRAL_DIR_HEADER_SIG = 0x02014b50, ZIP_LOCAL_DIR_HEADER_SIG = 0x04034b50,
	ZIP_LOCAL_DIR_HEADER_SIZE = 30, ZIP_CENTRAL_DIR_HEADER_SIZE = 46, ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE = 22,
	ZIP64_END_OF_CENTRAL_DIR_HEADER_SIG = 0x06064b50, ZIP64_END_OF_CENTRAL_DIR_HEADER_SIZE = 56,
	ZIP64_END_OF_CENTRAL_DIR_LOCATOR_SIG = 0x07064b50, ZIP64_END_OF_CENTRAL_DIR_LOCATOR_SIZE = 20,
	// End of central directory offsets
	ZIP_ECDH_NUM_THIS_DISK_OFS = 4, ZIP_ECDH_NUM_DISK_CDIR_OFS = 6, ZIP_ECDH_CDIR_NUM_ENTRIES_ON_DISK_OFS = 8,
	ZIP_ECDH_CDIR_TOTAL_ENTRIES_OFS = 10, ZIP_ECDH_CDIR_SIZE_OFS = 12, ZIP_ECDH_CDIR_OFS_OFS = 16, ZIP_ECDH_COMMENT_SIZE_OFS = 20,
	ZIP64_ECDL_ECDH_OFS_OFS = 8, ZIP64_ECDH_CDIR_TOTAL_ENTRIES_OFS = 32, ZIP64_ECDH_CDIR_SIZE_OFS = 40, ZIP64_ECDH_CDIR_OFS_OFS = 48,
	// Central directory header record offsets
	ZIP_CDH_BIT_FLAG_OFS = 8, ZIP_CDH_METHOD_OFS = 10, ZIP_CDH_FILE_TIME_OFS = 12, ZIP_CDH_FILE_DATE_OFS = 14, ZIP_CDH_CRC32_OFS = 16,
	ZIP_CDH_COMPRESSED_SIZE_OFS = 20, ZIP_CDH_DECOMPRESSED_SIZE_OFS = 24, ZIP_CDH_FILENAME_LEN_OFS = 28, ZIP_CDH_EXTRA_LEN_OFS = 30,
	ZIP_CDH_COMMENT_LEN_OFS = 32, ZIP_CDH_EXTERNAL_ATTR_OFS = 38, ZIP_CDH_LOCAL_HEADER_OFS = 42,
	// Local directory header offsets
	ZIP_LDH_FILENAME_LEN_OFS = 26, ZIP_LDH_EXTRA_LEN_OFS = 28,
};

#define ZIP_MAX(a,b) (((a)>(b))?(a):(b))
#define ZIP_MIN(a,b) (((a)<(b))?(a):(b))
#define ZIP_READ_LE16(p) ((Bit16u)(((const Bit8u *)(p))[0]) | ((Bit16u)(((const Bit8u *)(p))[1]) << 8U))
#define ZIP_READ_LE32(p) ((Bit32u)(((const Bit8u *)(p))[0]) | ((Bit32u)(((const Bit8u *)(p))[1]) << 8U) | ((Bit32u)(((const Bit8u *)(p))[2]) << 16U) | ((Bit32u)(((const Bit8u *)(p))[3]) << 24U))
#define ZIP_READ_LE64(p) ((Bit64u)(((const Bit8u *)(p))[0]) | ((Bit64u)(((const Bit8u *)(p))[1]) << 8U) | ((Bit64u)(((const Bit8u *)(p))[2]) << 16U) | ((Bit64u)(((const Bit8u *)(p))[3]) << 24U) | ((Bit64u)(((const Bit8u *)(p))[4]) << 32U) | ((Bit64u)(((const Bit8u *)(p))[5]) << 40U) | ((Bit64u)(((const Bit8u *)(p))[6]) << 48U) | ((Bit64u)(((const Bit8u *)(p))[7]) << 56U))
#define ZIP_READ_BE32(p) ((Bit32u)((((const Bit8u *)(p))[0] << 24) | (((const Bit8u *)(p))[1] << 16) | (((const Bit8u *)(p))[2] << 8) | ((const Bit8u *)(p))[3]))
#define ZIP_READ_BE64(p) ((Bit64u)((((Bit64u)((const Bit8u *)(p))[0] << 56) | ((Bit64u)((const Bit8u *)(p))[1] << 48) | ((Bit64u)((const Bit8u *)(p))[2] << 40) | ((Bit64u)((const Bit8u *)(p))[3] << 32) | ((Bit64u)((const Bit8u *)(p))[4] << 24) | ((Bit64u)((const Bit8u *)(p))[5] << 16) | ((Bit64u)((const Bit8u *)(p))[6] << 8) | (Bit64u)((const Bit8u *)(p))[7])))

#ifdef NDEBUG
#define ZIP_ASSERT(cond)
#else
#define ZIP_ASSERT(cond) (void)((cond) ? ((int)0) : *(volatile int*)0 |= 0xbad|fprintf(stderr, "FAILED ASSERT (%s)\n", #cond))
#endif

void XMLAppendRawF(std::vector<Bit8u>& buf, size_t maxlen, char const* format, ...)
{
	size_t oldlen = buf.size();
	buf.resize(oldlen + maxlen + 1);
	va_list va;
	va_start(va, format);
	int res = vsprintf((char*)&buf[oldlen], format, va);
	va_end(va);
	ZIP_ASSERT(maxlen >= (size_t)res);
	buf.resize(oldlen + res);
}

void XMLAppendRaw(std::vector<Bit8u>& buf, size_t len, const char* str)
{
	buf.resize(buf.size() + len);
	memcpy(&buf[buf.size() - len], str, len);
}

void XMLAppendEscaped(std::vector<Bit8u>& buf, size_t len, const char* str)
{
	for (; len--; str++)
		if (*str == '&') XMLAppendRaw(buf, 5, "&amp;");
		else if (*str == '<') XMLAppendRaw(buf, 4, "&lt;");
		else if (*str == '>') XMLAppendRaw(buf, 4, "&gt;");
		else buf.push_back((Bit8u)*str);
}

void ListCHDTracks(std::vector<Bit8u>& line, const Bit8u* chd_data, size_t chd_size)
{
	enum { CHD_V5_HEADER_SIZE = 124, CHD_V5_UNCOMPMAPENTRYBYTES = 4, CD_MAX_SECTOR_DATA = 2352, CD_MAX_SUBCODE_DATA = 96, CD_FRAME_SIZE = CD_MAX_SECTOR_DATA + CD_MAX_SUBCODE_DATA };
	enum { METADATA_HEADER_SIZE = 16, CDROM_TRACK_METADATA_TAG = 1128813650, CDROM_TRACK_METADATA2_TAG = 1128813618, CD_TRACK_PADDING = 4 };

	// Read CHD header and check signature
	Bit32u* chd_hunkmap = NULL;
	const Bit8u* rawheader = chd_data;
	if (chd_size < CHD_V5_HEADER_SIZE || memcmp(rawheader, "MComprHD", 8))
	{
		chderr:
		fprintf(stderr, "Invalid/unsupported CHD file\n");
		if (chd_hunkmap) free(chd_hunkmap);
		return;
	}

	// Check supported version, flags and compression
	Bit32u hdr_length = ZIP_READ_BE32(&rawheader[8]);
	Bit32u hdr_version = ZIP_READ_BE32(&rawheader[12]);
	if (hdr_version != 5 || hdr_length != CHD_V5_HEADER_SIZE) goto chderr; // only ver 5 is supported
	if (ZIP_READ_BE32(&rawheader[16])) goto chderr; // compression is not supported

	// Make sure it's a CD image
	Bit32u unitsize = ZIP_READ_BE32(&rawheader[60]);
	int chd_hunkbytes = (int)ZIP_READ_BE32(&rawheader[56]);
	if (unitsize != CD_FRAME_SIZE || (chd_hunkbytes % CD_FRAME_SIZE) || !chd_hunkbytes) goto chderr; // not CD sector size

	// Read file offsets for hunk mapping and track meta data
	Bit64u filelen = (Bit64u)chd_size;
	Bit64u logicalbytes = ZIP_READ_BE64(&rawheader[32]);
	Bit64u mapoffset = ZIP_READ_BE64(&rawheader[40]);
	Bit64u metaoffset = ZIP_READ_BE64(&rawheader[48]);
	if (mapoffset < CHD_V5_HEADER_SIZE || mapoffset >= filelen || metaoffset < CHD_V5_HEADER_SIZE || metaoffset >= filelen || !logicalbytes) goto chderr;

	// Read hunk mapping and convert to file offsets
	Bit32u hunkcount = (Bit32u)((logicalbytes + chd_hunkbytes - 1) / chd_hunkbytes);
	if (chd_size < mapoffset + hunkcount * CHD_V5_UNCOMPMAPENTRYBYTES) goto chderr;
	const Bit8u* raw_hunkmap = chd_data + (size_t)mapoffset;
	chd_hunkmap = (Bit32u*)malloc(hunkcount * CHD_V5_UNCOMPMAPENTRYBYTES);
	for (Bit32u i = 0; i != hunkcount; i++)
	{
		chd_hunkmap[i] = ZIP_READ_BE32(&raw_hunkmap[i * CHD_V5_UNCOMPMAPENTRYBYTES]) * chd_hunkbytes;
		if (chd_size < chd_hunkmap[i] + chd_hunkbytes) goto chderr;
	}

	// Read track meta data
	Bit32u track_frame = 0;
	for (Bit64u metaentry_offset = metaoffset, metaentry_next; metaentry_offset != 0; metaentry_offset = metaentry_next)
	{
		char mt_type[32], mt_subtype[32];
		if (chd_size < metaentry_offset + METADATA_HEADER_SIZE) goto chderr;
		const Bit8u* raw_meta_header = chd_data + metaentry_offset;
		Bit32u metaentry_metatag = ZIP_READ_BE32(&raw_meta_header[0]);
		Bit32u metaentry_length = (ZIP_READ_BE32(&raw_meta_header[4]) & 0x00ffffff);
		metaentry_next = ZIP_READ_BE64(&raw_meta_header[8]);
		if (metaentry_metatag != CDROM_TRACK_METADATA_TAG && metaentry_metatag != CDROM_TRACK_METADATA2_TAG) continue;
		if (chd_size < (size_t)(metaentry_offset + METADATA_HEADER_SIZE) + metaentry_length) goto chderr;
		const char* meta = (const char*)(chd_data + metaentry_offset + METADATA_HEADER_SIZE);

		int mt_track_no = 0, mt_frames = 0, mt_pregap = 0;
		if (sscanf(meta,
			(metaentry_metatag == CDROM_TRACK_METADATA2_TAG ? "TRACK:%d TYPE:%30s SUBTYPE:%30s FRAMES:%d PREGAP:%d" : "TRACK:%d TYPE:%30s SUBTYPE:%30s FRAMES:%d"),
			&mt_track_no, mt_type, mt_subtype, &mt_frames, &mt_pregap) < 4) continue;

		// In CHD files tracks are padded to a to a 4-sector boundary.
		track_frame += ((CD_TRACK_PADDING - (track_frame % CD_TRACK_PADDING)) % CD_TRACK_PADDING);

		// Read track data and calculate hashes (CHD sectorSize is always 2448, data_size is based on chdman source)
		const bool isAudio = !strcmp(mt_type, "AUDIO");
		const bool ds2048 = !strcmp(mt_type, "MODE1") || !strcmp(mt_type, "MODE2_FORM1");
		const bool ds2324 = !strcmp(mt_type, "MODE2_FORM2");
		const bool ds2336 = !strcmp(mt_type, "MODE2") || !strcmp(mt_type, "MODE2_FORM_MIX");
		const size_t data_size = (ds2048 ? 2048 : ds2324 ? 2324 : ds2336 ? 2336 : CD_MAX_SECTOR_DATA);
		const size_t track_size = (size_t)mt_frames * data_size;
		Bit8u* track_data = (Bit8u*)malloc(track_size), *track_out = track_data;
		for (Bit32u track_frame_end = track_frame + mt_frames; track_frame != track_frame_end; track_frame++, track_out += data_size)
		{
			size_t p = track_frame * CD_FRAME_SIZE, hunk = (p / chd_hunkbytes), hunk_ofs = (p % chd_hunkbytes), hunk_pos = chd_hunkmap[hunk];
			if (!hunk_pos) { memset(track_out, 0, data_size); }
			else { memcpy(track_out, chd_data + hunk_pos + hunk_ofs, data_size); }
		}
		if (isAudio) // CHD audio endian swap
			for (Bit8u *p = track_data, *pEnd = p + track_size, tmp; p != pEnd; p += 2)
				{ tmp = p[0]; p[0] = p[1]; p[1] = tmp; }
		Bit8u trackmd5[16], tracksha1[20];
		FastMD5(track_data, (size_t)track_size, trackmd5);
		SHA1(track_data, (size_t)track_size, tracksha1);
		Bit32u trackcrc32 = CRC32(track_data, (size_t)track_size), in_zeros, out_zeros;
		for (in_zeros = 0; in_zeros != track_size && track_data[in_zeros] == 0; in_zeros++) {}
		for (out_zeros = 0; out_zeros != track_size && track_data[track_size - 1 - out_zeros] == 0; out_zeros++) {}
		free(track_data);

		XMLAppendRawF(line, 200, "			<track number=\"%d\" type=\"%s\" frames=\"%d\" pregap=\"%d\" duration=\"%02d:%02d:%02d\" size=\"%u\"",
			mt_track_no, mt_type, mt_frames, mt_pregap, (mt_frames/75/60), (mt_frames/75)%60, mt_frames%75, (Bit32u)track_size);
		XMLAppendRawF(line, 21, " crc=\"%08x\" md5=\"", trackcrc32);
		for (int md5i = 0; md5i != 16; md5i++) XMLAppendRawF(line, 2, "%02x", trackmd5[md5i]);
		XMLAppendRaw(line, 8, "\" sha1=\"");
		for (int sha1i = 0; sha1i != 20; sha1i++) XMLAppendRawF(line, 2, "%02x", tracksha1[sha1i]);
		XMLAppendRawF(line, 64, "\" in_zeros=\"%u\" out_zeros=\"%u\"/>\n", in_zeros, out_zeros);
	}

	free(chd_hunkmap);
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Missing DOSZ path(s)\n\nRun tool with:\n  %s <DOSZ PATH> ...\n\n", (argc ? argv[0] : "MakePureDOSDat"));
		return 1;
	}
	for (int test = 1; test != argc; test++)
	{
		FILE *fZIP = fopen(argv[test], "rb");
		if (!fZIP)
		{
			fprintf(stderr, "Unable to find input file %s\n", argv[test]);
			return 1;
		}
		fclose(fZIP);
	}

	time_t t = time(NULL);
	struct tm tm = *gmtime(&t);
	printf("<?xml version=\"1.0\"?>\n");
	printf("<!DOCTYPE datafile PUBLIC \"-//Logiqx//DTD ROM Management Datafile//EN\" \"http://www.logiqx.com/Dats/datafile.dtd\">\n");
	printf("<datafile>\n");
	printf("	<header>\n");
	printf("		<name>Pure DOS DAT</name>\n");
	printf("		<description>DOS Games</description>\n");
	printf("		<version>%04d-%02d-%02d %02d-%02d-%02d</version>\n", (tm.tm_year+1900), (tm.tm_mon+1), tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	printf("		<date>%04d-%02d-%02d %02d-%02d-%02d</date>\n", (tm.tm_year+1900), (tm.tm_mon+1), tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	printf("		<author>Pure DOS DAT</author>\n");
	printf("		<homepage>Pure DOS DAT</homepage>\n");
	printf("		<url>https://github.com/PureDOS/DAT</url>\n");
	printf("	</header>\n");

	for (int argi = 1; argi != argc; argi++)
	{
		const char *path = argv[argi];
		const char *lasts = strrchr(path, '/'), *lastbs = strrchr(path, '\\');
		const char *fname = (lasts > lastbs && lasts[1] ? lasts+1 : (lastbs && lastbs[1] ? lastbs+1 : path));
		const char *pathend = fname + strlen(fname), *fname_gameend = pathend, *ext = NULL;
		for (const char* pDot = fname_gameend;; fname_gameend = pDot--)
		{
			while (pDot > fname && *pDot != '.') pDot--;
			if (pDot == fname) break;
			else if (!strncasecmp(pDot, ".dosz", 5)) ext = pDot;
			else if (!strncasecmp(pDot, ".dosc", 5)) ext = pDot;
			else if (!strncasecmp(pDot, ".zip", 4)) ext = pDot;
			else break;
		}
		int year = 0;
		const char *developer = NULL, *developerEd = NULL, *variant = NULL, *variantEd = NULL;
		for (const char *op, *ed = fname; (op = strstr(ed+1, " (")) != NULL && (ed = strchr(op, ')')) != NULL;)
		{
			if (op < fname_gameend) fname_gameend = op;
			if (year < 1970 && (ed - op) == 6 && (year = atoi(op+2)) >= 1970) { }
			else if (!developer) { developer = op+2; developerEd = ed; }
			else if (!variant) { variant = op+2; variantEd = ed; }
		}

		FILE *fZIP = fopen(path, "rb");
		if (!fZIP)
		{
			fprintf(stderr, "Unable to find input file %s\n", path);
			return 1;
		}

		FILE *fTXT = NULL, *fDOSCTXT = NULL;
		if (ext)
		{
			char* tmp = (char*)malloc(pathend - path + 5), *tmpzc = tmp + (ext - path) + 4;
			strcpy(tmp, path);
			if (tmpzc[0] && tmpzc[1] == '.') tmpzc[1] = '\0'; // cut between ".dosz.zip"
			strcat(tmp, ".txt");
			fTXT = fopen(tmp, "rb");
			if ((tmpzc[0]|0x20) == 'z')
			{
				*tmpzc = (*tmpzc == 'z' ? 'c' : 'C');
				fDOSCTXT = fopen(tmp, "rb");
			}
			free(tmp);
		}

		std::vector<Bit8u> gametag;
		XMLAppendRaw(gametag, 13, "	<game name=\""); XMLAppendEscaped(gametag, ((ext ? ext : pathend)-fname), fname); XMLAppendRaw(gametag, 8, (ext && (ext[4]|0x20) == 'c' ? ".dosc\">\n" : ".dosz\">\n"));
		XMLAppendRaw(gametag, 15, "		<description>"); XMLAppendEscaped(gametag, (fname_gameend-fname), fname); XMLAppendRaw(gametag, 15, "</description>\n");
		if (fTXT)
		{
			XMLAppendRaw(gametag, 11, "		<comment>");
			for (int n, c[4]; (n = fread(c, 1, sizeof(c), fTXT)) != 0;) XMLAppendEscaped(gametag, n, (char*)c);
			XMLAppendRaw(gametag, 11, "</comment>\n");
			fclose(fTXT);
		}
		if (fDOSCTXT)
		{
			XMLAppendRawF(gametag, 16, "		<comment_dosc>");
			for (int n, c[4]; (n = fread(c, 1, sizeof(c), fDOSCTXT)) != 0;) XMLAppendEscaped(gametag, n, (char*)c);
			XMLAppendRawF(gametag, 16, "</comment_dosc>\n");
			fclose(fDOSCTXT);
		}
		if (year >= 1970 && year <= 9999) XMLAppendRawF(gametag, 20, "		<year>%d</year>\n", year);
		if (developer) { XMLAppendRaw(gametag, 13, "		<developer>"); XMLAppendEscaped(gametag, (developerEd-developer), developer); XMLAppendRaw(gametag, 13, "</developer>\n"); }
		if (variant)   { XMLAppendRaw(gametag, 11, "		<variant>"); XMLAppendEscaped(gametag, (variantEd-variant), variant); XMLAppendRaw(gametag, 11, "</variant>\n"); }
		printf("%.*s", (unsigned)gametag.size(), (char*)&gametag[0]);

		Zip_Archive archive(fZIP);

		// Basic sanity checks - reject files which are too small.
		if (archive.size < ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE)
			return 1;

		// Find the end of central directory record by scanning the file from the end towards the beginning.
		Bit8u buf[4096];
		Bit64u ecdh_ofs = (archive.size < sizeof(buf) ? 0 : archive.size - sizeof(buf));
		for (;; ecdh_ofs = ZIP_MAX(ecdh_ofs - (sizeof(buf) - 3), 0))
		{
			Bit32s i, n = (Bit32s)ZIP_MIN(sizeof(buf), archive.size - ecdh_ofs);
			if (archive.Read(ecdh_ofs, buf, (Bit32u)n) != (Bit32u)n) return 1;
			for (i = n - 4; i >= 0; --i) { if (ZIP_READ_LE32(buf + i) == ZIP_END_OF_CENTRAL_DIR_HEADER_SIG) break; }
			if (i >= 0) { ecdh_ofs += i; break; }
			if (!ecdh_ofs || (archive.size - ecdh_ofs) >= (0xFFFF + ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE)) return 1;
		}

		// Read and verify the end of central directory record.
		if (archive.Read(ecdh_ofs, buf, ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE) != ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE)
			return 1;

		Bit64u total_files = ZIP_READ_LE16(buf + ZIP_ECDH_CDIR_TOTAL_ENTRIES_OFS);
		Bit64u cdir_size   = ZIP_READ_LE32(buf + ZIP_ECDH_CDIR_SIZE_OFS);
		Bit64u cdir_ofs    = ZIP_READ_LE32(buf + ZIP_ECDH_CDIR_OFS_OFS);

		if ((cdir_ofs == 0xFFFFFFFF || cdir_size == 0xFFFFFFFF || total_files == 0xFFFF)
			&& ecdh_ofs >= (ZIP64_END_OF_CENTRAL_DIR_LOCATOR_SIZE + ZIP64_END_OF_CENTRAL_DIR_HEADER_SIZE)
			&& archive.Read(ecdh_ofs - ZIP64_END_OF_CENTRAL_DIR_LOCATOR_SIZE, buf, ZIP64_END_OF_CENTRAL_DIR_LOCATOR_SIZE) == ZIP64_END_OF_CENTRAL_DIR_LOCATOR_SIZE
			&& ZIP_READ_LE32(buf) == ZIP64_END_OF_CENTRAL_DIR_LOCATOR_SIG)
		{
			Bit64u ecdh64_ofs = ZIP_READ_LE64(buf + ZIP64_ECDL_ECDH_OFS_OFS);
			if (ecdh64_ofs <= (archive.size - ZIP64_END_OF_CENTRAL_DIR_HEADER_SIZE)
				&& archive.Read(ecdh64_ofs, buf, ZIP64_END_OF_CENTRAL_DIR_HEADER_SIZE) == ZIP64_END_OF_CENTRAL_DIR_HEADER_SIZE
				&& ZIP_READ_LE32(buf) == ZIP64_END_OF_CENTRAL_DIR_HEADER_SIG)
			{
				total_files = ZIP_READ_LE64(buf + ZIP64_ECDH_CDIR_TOTAL_ENTRIES_OFS);
				cdir_size   = ZIP_READ_LE64(buf + ZIP64_ECDH_CDIR_SIZE_OFS);
				cdir_ofs    = ZIP_READ_LE64(buf + ZIP64_ECDH_CDIR_OFS_OFS);
			}
		}

		if (!total_files
			|| (cdir_size >= 0x10000000) // limit to 256MB content directory
			|| (cdir_size < total_files * ZIP_CENTRAL_DIR_HEADER_SIZE)
			|| ((cdir_ofs + cdir_size) > archive.size)
			) return 1;

		void* m_central_dir = malloc((size_t)cdir_size);
		if (archive.Read(cdir_ofs, m_central_dir, (Bit32u)cdir_size) != cdir_size)
		{
			free(m_central_dir);
			return 1;
		}
		const Bit8u *cdir_start = (const Bit8u*)m_central_dir, *cdir_end = cdir_start + cdir_size, *p = cdir_start;

		// Now create an index into the central directory file records, do some basic sanity checking on each record, and check for zip64 entries (which are not yet supported).
		p = cdir_start;
		std::vector< std::vector<Bit8u> > roms;
		for (Bit32u i = 0, total_header_size; i < total_files && p >= cdir_start && p < cdir_end && ZIP_READ_LE32(p) == ZIP_CENTRAL_DIR_HEADER_SIG; i++, p += total_header_size)
		{
			Bit32u bit_flag         = ZIP_READ_LE16(p + ZIP_CDH_BIT_FLAG_OFS);
			Bit32u method           = ZIP_READ_LE16(p + ZIP_CDH_METHOD_OFS);
			Bit16u file_time        = ZIP_READ_LE16(p + ZIP_CDH_FILE_TIME_OFS);
			Bit16u file_date        = ZIP_READ_LE16(p + ZIP_CDH_FILE_DATE_OFS);
			Bit32u crc32            = ZIP_READ_LE32(p + ZIP_CDH_CRC32_OFS);
			Bit64u comp_size        = ZIP_READ_LE32(p + ZIP_CDH_COMPRESSED_SIZE_OFS);
			Bit64u decomp_size      = ZIP_READ_LE32(p + ZIP_CDH_DECOMPRESSED_SIZE_OFS);
			Bit32u filename_len     = ZIP_READ_LE16(p + ZIP_CDH_FILENAME_LEN_OFS);
			Bit32s extra_len        = ZIP_READ_LE16(p + ZIP_CDH_EXTRA_LEN_OFS);
			Bit32s external_attr    = ZIP_READ_LE32(p + ZIP_CDH_EXTERNAL_ATTR_OFS);
			Bit64u local_header_ofs = ZIP_READ_LE32(p + ZIP_CDH_LOCAL_HEADER_OFS);
			total_header_size = ZIP_CENTRAL_DIR_HEADER_SIZE + filename_len + extra_len + ZIP_READ_LE16(p + ZIP_CDH_COMMENT_LEN_OFS);

			if (!Zip_Archive::MethodSupported(method)
				|| (p + total_header_size > cdir_end)
				|| (bit_flag & (1 | 32)) // Encryption and patch files are not supported.
				) { invalid_cdh: continue; }

			if (decomp_size == 0xFFFFFFFF || comp_size == 0xFFFFFFFF || local_header_ofs == 0xFFFFFFFF)
			{
				for (const Bit8u *x = p + ZIP_CENTRAL_DIR_HEADER_SIZE + filename_len, *xEnd = x + extra_len; (x + (sizeof(Bit16u) * 2)) < xEnd;)
				{
					const Bit8u *field = x + (sizeof(Bit16u) * 2), *fieldEnd = field + ZIP_READ_LE16(x + 2);
					if (ZIP_READ_LE16(x) != 0x0001 || fieldEnd > xEnd) { x = fieldEnd; continue; } // Not Zip64 extended information extra field
					if (decomp_size == 0xFFFFFFFF)
					{
						if ((size_t)(fieldEnd - field) < sizeof(Bit64u)) goto invalid_cdh;
						decomp_size = ZIP_READ_LE64(field);
						field += sizeof(Bit64u);
					}
					if (comp_size == 0xFFFFFFFF)
					{
						if ((size_t)(fieldEnd - field) < sizeof(Bit64u)) goto invalid_cdh;
						comp_size = ZIP_READ_LE64(field);
						field += sizeof(Bit64u);
					}
					if (local_header_ofs == 0xFFFFFFFF)
					{
						if ((size_t)(fieldEnd - field) < sizeof(Bit64u)) goto invalid_cdh;
						local_header_ofs = ZIP_READ_LE64(field);
						field += sizeof(Bit64u);
					}
					break;
				}
			}

			if (((!method) && (decomp_size != comp_size)) || (decomp_size && !comp_size)
				|| (decomp_size > 0xFFFFFFFF) || (comp_size > 0xFFFFFFFF) // not supported on DOS file systems
				|| ((local_header_ofs + ZIP_LOCAL_DIR_HEADER_SIZE + comp_size) > archive.size)
				) continue;

			std::vector<Bit8u> mem_data;
			archive.Unpack(local_header_ofs, (Bit32u)comp_size, (Bit32u)decomp_size, (Bit8u)bit_flag, (Bit8u)method, mem_data);
			const Bit8u* mem_ptr = (decomp_size ? &mem_data[0] : NULL);
			ZIP_ASSERT(mem_data.size() == decomp_size);
			ZIP_ASSERT(crc32 == CRC32(mem_ptr, (size_t)decomp_size));

			Bit8u md5[16], sha1[20];
			FastMD5(mem_ptr, (size_t)decomp_size, md5);
			SHA1(mem_ptr, (size_t)decomp_size, sha1);

			char *name = (char*)(p + ZIP_CENTRAL_DIR_HEADER_SIZE);
			for (char *p = name, *name_end = name + filename_len; p != name_end; p++)
				if (*p == '\\')
					*p = '/'; // convert back-slashes to regular slashes

			bool is_dir = (name[filename_len - 1] == '/' || (external_attr & 0x10));
			if (is_dir && (file_date >> 9) >= 44)
			{
				// Fix time stamp of directory records from 2024 and newer to 1999-12-31 (and skip them when outputting records of non-empty directories below)
				file_date = ((19 << 9) | (12 << 5) | 31);
				file_time = 0;
			}
			else if (file_date == 8600 && file_time == 48128)
			{
				// Don't output the TrrntZip default time stamp (1996-12-24 23:32:00)
				file_date = file_time = 0;
			}
			else if ((file_date >> 9) >= 44)
			{
				// Don't output file time stamps from 2024 and newer
				file_date = file_time = 0;
			}

			// Terminate file name with \b for sorting below (is changed to " during output)
			roms.push_back(std::vector<Bit8u>());
			std::vector<Bit8u>& line = roms.back();
			XMLAppendRaw(line, 13, "		<rom name=\"");
			XMLAppendEscaped(line, filename_len, name);
			if (is_dir && name[filename_len - 1] != '/') line.push_back('/');
			XMLAppendRawF(line, 41, "\b size=\"%u\" crc=\"%08x\" md5=\"", (unsigned)decomp_size, crc32);
			for (int md5i = 0; md5i != 16; md5i++) XMLAppendRawF(line, 2, "%02x", md5[md5i]);
			XMLAppendRaw(line, 8, "\" sha1=\"");
			for (int sha1i = 0; sha1i != 20; sha1i++) XMLAppendRawF(line, 2, "%02x", sha1[sha1i]);
			if (file_date || file_time)
				XMLAppendRawF(line, 27, "\" date=\"%04d-%02d-%02d %02d:%02d:%02d", ((file_date >> 9) + 1980), ((file_date >> 5) & 0xf), (file_date & 0x1f), (file_time >> 11), ((file_time >> 5) & 0x3f), ((file_time & 0x1f) * 2));

			// If this is a CHD file, print out track information
			if (filename_len > 4 && !strncasecmp(name + filename_len - 4, ".chd", 4))
			{
				XMLAppendRaw(line, 3, "\">\n");
				ListCHDTracks(line, mem_ptr, (size_t)decomp_size);
				XMLAppendRaw(line, 9, "		</rom>\n");
			}
			else
			{
				XMLAppendRaw(line, 4, "\"/>\n");
			}
		}
		free(m_central_dir);

		// Sort strings (which sorts it by filename)
		struct Local
		{
			static int SortFunc(const void* va, const void* vb)
			{
				std::vector<Bit8u> &a = *(std::vector<Bit8u>*)va, &b = *(std::vector<Bit8u>*)vb;
				for (char *pa = (char*)&a[0], *pb = (char*)&b[0];;)
				{
					char ca = *(pa++), cb = *(pb++);
					int d = ((ca >= 'A' && ca <= 'Z') ? (ca|0x20) : ca) - ((cb >= 'A' && cb <= 'Z') ? (cb|0x20) : cb);
					if (d) return d;
					if (!ca) return 0;
				}
			}
		};
		qsort(&roms[0], roms.size(), sizeof(roms[0]), Local::SortFunc);

		// Output rom strings (except directories)
		for (size_t j = 0, iend = roms.size(); j != iend; j++)
		{
			char *line = (char*)&roms[j][0],  *name_end = strchr(line, '\b');
			if (name_end[-1] == '/')
			{
				// Don't print rom element for non-empty directory record with a too new time stamp
				bool empty_dir = (j == iend - 1 || memcmp(line, &roms[j+1][0], (name_end - line)));
				bool ignore_dir = (!empty_dir && (strstr(line, " date=\"1999-12-31 00:00:00\"") || !strstr(line, " date=\"")));
				if (ignore_dir)
					continue;
			}
			*name_end = '"'; // fix \b to " now that entries are sorted and directories are checked
			printf("%.*s", (unsigned)roms[j].size(), line);
		}

		printf("	</game>\n");
	}
	printf("</datafile>\n");
	return 0;
}

struct miniz
{
	// BASED ON MINIZ
	// miniz.c v1.15 - public domain deflate
	// Rich Geldreich <richgel99@gmail.com>, last updated Oct. 13, 2013

	// Set MINIZ_HAS_64BIT_REGISTERS to 1 if operations on 64-bit integers are reasonably fast (and don't involve compiler generated calls to helper functions).
	#if defined(_M_X64) || defined(_WIN64) || defined(__MINGW64__) || defined(_LP64) || defined(__LP64__) || defined(__ia64__) || defined(__x86_64__)
	#define MINIZ_HAS_64BIT_REGISTERS 1
	#else
	#define MINIZ_HAS_64BIT_REGISTERS 0
	#endif

	enum
	{
		// Decompression flags used by tinfl_decompress().
		TINFL_FLAG_HAS_MORE_INPUT = 2,                // If set, there are more input bytes available beyond the end of the supplied input buffer. If clear, the input buffer contains all remaining input.
		TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF = 4, // If set, the output buffer is large enough to hold the entire decompressed stream. If clear, the output buffer is at least the size of the dictionary (typically 32KB).

		// Max size of read buffer.
		MZ_ZIP_MAX_IO_BUF_SIZE = 16*1024, // Was 64*1024 originally (though max size readable through DOS_File would be 0xFFFF).

		// Max size of LZ dictionary (output buffer).
		TINFL_LZ_DICT_SIZE = 32*1024, // fixed for zip

		// Internal/private bits follow.
		TINFL_MAX_HUFF_TABLES = 3, TINFL_MAX_HUFF_SYMBOLS_0 = 288, TINFL_MAX_HUFF_SYMBOLS_1 = 32, TINFL_MAX_HUFF_SYMBOLS_2 = 19,
		TINFL_FAST_LOOKUP_BITS = 10, TINFL_FAST_LOOKUP_SIZE = 1 << TINFL_FAST_LOOKUP_BITS,

		// Number coroutine states consecutively
		TINFL_STATE_INDEX_BLOCK_BOUNDRY = 1,
		TINFL_STATE_3 , TINFL_STATE_5 , TINFL_STATE_6 , TINFL_STATE_7 , TINFL_STATE_51, TINFL_STATE_52,
		TINFL_STATE_9 , TINFL_STATE_38, TINFL_STATE_11, TINFL_STATE_14, TINFL_STATE_16, TINFL_STATE_18,
		TINFL_STATE_23, TINFL_STATE_24, TINFL_STATE_25, TINFL_STATE_26, TINFL_STATE_27, TINFL_STATE_53,
		TINFL_STATE_END
	};

	// Return status.
	enum tinfl_status
	{
		TINFL_STATUS_BAD_PARAM = -3,
		TINFL_STATUS_FAILED = -1,
		TINFL_STATUS_DONE = 0,
		TINFL_STATUS_NEEDS_MORE_INPUT = 1,
		TINFL_STATUS_HAS_MORE_OUTPUT = 2,
	};

	#if MINIZ_HAS_64BIT_REGISTERS
	typedef Bit64u tinfl_bit_buf_t;
	#else
	typedef Bit32u tinfl_bit_buf_t;
	#endif

	struct tinfl_huff_table
	{
		Bit16s m_look_up[TINFL_FAST_LOOKUP_SIZE];
		Bit16s m_tree[TINFL_MAX_HUFF_SYMBOLS_0 * 2];
		Bit8u m_code_size[TINFL_MAX_HUFF_SYMBOLS_0];
	};

	struct tinfl_decompressor
	{
		tinfl_huff_table m_tables[TINFL_MAX_HUFF_TABLES];
		Bit32u m_state, m_num_bits, m_final, m_type, m_dist, m_counter, m_num_extra, m_table_sizes[TINFL_MAX_HUFF_TABLES];
		tinfl_bit_buf_t m_bit_buf;
		size_t m_dist_from_out_buf_start;
		Bit8u m_raw_header[4], m_len_codes[TINFL_MAX_HUFF_SYMBOLS_0 + TINFL_MAX_HUFF_SYMBOLS_1 + 137];
	};

	// Initializes the decompressor to its initial state.
	static void tinfl_init(tinfl_decompressor *r) { r->m_state = 0; }

	// Main low-level decompressor coroutine function. This is the only function actually needed for decompression. All the other functions are just high-level helpers for improved usability.
	// This is a universal API, i.e. it can be used as a building block to build any desired higher level decompression API. In the limit case, it can be called once per every byte input or output.
	static tinfl_status tinfl_decompress(tinfl_decompressor *r, const Bit8u *pIn_buf_next, Bit32u *pIn_buf_size, Bit8u *pOut_buf_start, Bit8u *pOut_buf_next, Bit32u *pOut_buf_size, const Bit32u decomp_flags)
	{
		// An attempt to work around MSVC's spammy "warning C4127: conditional expression is constant" message.
		#ifdef _MSC_VER
		#define TINFL_MACRO_END while (0, 0)
		#else
		#define TINFL_MACRO_END while (0)
		#endif

		#define TINFL_MEMCPY(d, s, l) memcpy(d, s, l)
		#define TINFL_MEMSET(p, c, l) memset(p, c, l)
		#define TINFL_CLEAR(obj) memset(&(obj), 0, sizeof(obj))

		#define TINFL_CR_BEGIN switch(r->m_state) { case 0:
		#define TINFL_CR_RETURN(state_index, result) do { status = result; r->m_state = state_index; goto common_exit; case state_index:; } TINFL_MACRO_END
		#define TINFL_CR_RETURN_FOREVER(state_index, result) do { status = result; r->m_state = TINFL_STATE_END; goto common_exit; } TINFL_MACRO_END
		#define TINFL_CR_FINISH }

		// TODO: If the caller has indicated that there's no more input, and we attempt to read beyond the input buf, then something is wrong with the input because the inflator never
		// reads ahead more than it needs to. Currently TINFL_GET_BYTE() pads the end of the stream with 0's in this scenario.
		#define TINFL_GET_BYTE(state_index, c) do { \
			if (pIn_buf_cur >= pIn_buf_end) { \
				for ( ; ; ) { \
					if (decomp_flags & TINFL_FLAG_HAS_MORE_INPUT) { \
						TINFL_CR_RETURN(state_index, TINFL_STATUS_NEEDS_MORE_INPUT); \
						if (pIn_buf_cur < pIn_buf_end) { \
							c = *pIn_buf_cur++; \
							break; \
						} \
					} else { \
						c = 0; \
						break; \
					} \
				} \
			} else c = *pIn_buf_cur++; } TINFL_MACRO_END

		#define TINFL_NEED_BITS(state_index, n) do { Bit32u c; TINFL_GET_BYTE(state_index, c); bit_buf |= (((tinfl_bit_buf_t)c) << num_bits); num_bits += 8; } while (num_bits < (Bit32u)(n))
		#define TINFL_SKIP_BITS(state_index, n) do { if (num_bits < (Bit32u)(n)) { TINFL_NEED_BITS(state_index, n); } bit_buf >>= (n); num_bits -= (n); } TINFL_MACRO_END
		#define TINFL_GET_BITS(state_index, b, n) do { if (num_bits < (Bit32u)(n)) { TINFL_NEED_BITS(state_index, n); } b = bit_buf & ((1 << (n)) - 1); bit_buf >>= (n); num_bits -= (n); } TINFL_MACRO_END

		// TINFL_HUFF_BITBUF_FILL() is only used rarely, when the number of bytes remaining in the input buffer falls below 2.
		// It reads just enough bytes from the input stream that are needed to decode the next Huffman code (and absolutely no more). It works by trying to fully decode a
		// Huffman code by using whatever bits are currently present in the bit buffer. If this fails, it reads another byte, and tries again until it succeeds or until the
		// bit buffer contains >=15 bits (deflate's max. Huffman code size).
		#define TINFL_HUFF_BITBUF_FILL(state_index, pHuff) \
			do { \
				temp = (pHuff)->m_look_up[bit_buf & (TINFL_FAST_LOOKUP_SIZE - 1)]; \
				if (temp >= 0) { \
					code_len = temp >> 9; \
					if ((code_len) && (num_bits >= code_len)) \
					break; \
				} else if (num_bits > TINFL_FAST_LOOKUP_BITS) { \
					 code_len = TINFL_FAST_LOOKUP_BITS; \
					 do { \
							temp = (pHuff)->m_tree[~temp + ((bit_buf >> code_len++) & 1)]; \
					 } while ((temp < 0) && (num_bits >= (code_len + 1))); if (temp >= 0) break; \
				} TINFL_GET_BYTE(state_index, c); bit_buf |= (((tinfl_bit_buf_t)c) << num_bits); num_bits += 8; \
			} while (num_bits < 15);

		// TINFL_HUFF_DECODE() decodes the next Huffman coded symbol. It's more complex than you would initially expect because the zlib API expects the decompressor to never read
		// beyond the final byte of the deflate stream. (In other words, when this macro wants to read another byte from the input, it REALLY needs another byte in order to fully
		// decode the next Huffman code.) Handling this properly is particularly important on raw deflate (non-zlib) streams, which aren't followed by a byte aligned adler-32.
		// The slow path is only executed at the very end of the input buffer.
		#define TINFL_HUFF_DECODE(state_index, sym, pHuff) do { \
			int temp; Bit32u code_len, c; \
			if (num_bits < 15) { \
				if ((pIn_buf_end - pIn_buf_cur) < 2) { \
					 TINFL_HUFF_BITBUF_FILL(state_index, pHuff); \
				} else { \
					 bit_buf |= (((tinfl_bit_buf_t)pIn_buf_cur[0]) << num_bits) | (((tinfl_bit_buf_t)pIn_buf_cur[1]) << (num_bits + 8)); pIn_buf_cur += 2; num_bits += 16; \
				} \
			} \
			if ((temp = (pHuff)->m_look_up[bit_buf & (TINFL_FAST_LOOKUP_SIZE - 1)]) >= 0) \
				code_len = temp >> 9, temp &= 511; \
			else { \
				code_len = TINFL_FAST_LOOKUP_BITS; do { temp = (pHuff)->m_tree[~temp + ((bit_buf >> code_len++) & 1)]; } while (temp < 0); \
			} sym = temp; bit_buf >>= code_len; num_bits -= code_len; } TINFL_MACRO_END

		static const int s_length_base[31] = { 3,4,5,6,7,8,9,10,11,13, 15,17,19,23,27,31,35,43,51,59, 67,83,99,115,131,163,195,227,258,0,0 };
		static const int s_length_extra[31]= { 0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0,0,0 };
		static const int s_dist_base[32] = { 1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193, 257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577,0,0};
		static const int s_dist_extra[32] = { 0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};
		static const Bit8u s_length_dezigzag[19] = { 16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15 };
		static const int s_min_table_sizes[3] = { 257, 1, 4 };

		tinfl_status status = TINFL_STATUS_FAILED; Bit32u num_bits, dist, counter, num_extra; tinfl_bit_buf_t bit_buf;
		const Bit8u *pIn_buf_cur = pIn_buf_next, *const pIn_buf_end = pIn_buf_next + *pIn_buf_size, *const pIn_buf_end_m_4 = pIn_buf_end - 4;
		Bit8u *pOut_buf_cur = pOut_buf_next, *const pOut_buf_end = pOut_buf_next + *pOut_buf_size, *const pOut_buf_end_m_2 = pOut_buf_end - 2;
		size_t out_buf_size_mask = (decomp_flags & TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF) ? (size_t)-1 : ((pOut_buf_next - pOut_buf_start) + *pOut_buf_size) - 1, dist_from_out_buf_start;

		Bit16s* r_tables_0_look_up = r->m_tables[0].m_look_up;

		// Ensure the output buffer's size is a power of 2, unless the output buffer is large enough to hold the entire output file (in which case it doesn't matter).
		if (((out_buf_size_mask + 1) & out_buf_size_mask) || (pOut_buf_next < pOut_buf_start)) { *pIn_buf_size = *pOut_buf_size = 0; return TINFL_STATUS_BAD_PARAM; }

		num_bits = r->m_num_bits; bit_buf = r->m_bit_buf; dist = r->m_dist; counter = r->m_counter; num_extra = r->m_num_extra; dist_from_out_buf_start = r->m_dist_from_out_buf_start;
		TINFL_CR_BEGIN

		bit_buf = num_bits = dist = counter = num_extra = 0;

		do
		{
			if (pIn_buf_cur - pIn_buf_next) { TINFL_CR_RETURN(TINFL_STATE_INDEX_BLOCK_BOUNDRY, TINFL_STATUS_HAS_MORE_OUTPUT); }
			TINFL_GET_BITS(TINFL_STATE_3, r->m_final, 3); r->m_type = r->m_final >> 1;
			if (r->m_type == 0)
			{
				TINFL_SKIP_BITS(TINFL_STATE_5, num_bits & 7);
				for (counter = 0; counter < 4; ++counter) { if (num_bits) TINFL_GET_BITS(TINFL_STATE_6, r->m_raw_header[counter], 8); else TINFL_GET_BYTE(TINFL_STATE_7, r->m_raw_header[counter]); }
				if ((counter = (r->m_raw_header[0] | (r->m_raw_header[1] << 8))) != (Bit32u)(0xFFFF ^ (r->m_raw_header[2] | (r->m_raw_header[3] << 8)))) { TINFL_CR_RETURN_FOREVER(39, TINFL_STATUS_FAILED); }
				while ((counter) && (num_bits))
				{
					TINFL_GET_BITS(TINFL_STATE_51, dist, 8);
					while (pOut_buf_cur >= pOut_buf_end) { TINFL_CR_RETURN(TINFL_STATE_52, TINFL_STATUS_HAS_MORE_OUTPUT); }
					*pOut_buf_cur++ = (Bit8u)dist;
					counter--;
				}
				while (counter)
				{
					size_t n; while (pOut_buf_cur >= pOut_buf_end) { TINFL_CR_RETURN(TINFL_STATE_9, TINFL_STATUS_HAS_MORE_OUTPUT); }
					while (pIn_buf_cur >= pIn_buf_end)
					{
						if (decomp_flags & TINFL_FLAG_HAS_MORE_INPUT)
						{
							TINFL_CR_RETURN(TINFL_STATE_38, TINFL_STATUS_NEEDS_MORE_INPUT);
						}
						else
						{
							TINFL_CR_RETURN_FOREVER(40, TINFL_STATUS_FAILED);
						}
					}
					n = ZIP_MIN(ZIP_MIN((size_t)(pOut_buf_end - pOut_buf_cur), (size_t)(pIn_buf_end - pIn_buf_cur)), counter);
					TINFL_MEMCPY(pOut_buf_cur, pIn_buf_cur, n); pIn_buf_cur += n; pOut_buf_cur += n; counter -= (Bit32u)n;
				}
			}
			else if (r->m_type == 3)
			{
				TINFL_CR_RETURN_FOREVER(10, TINFL_STATUS_FAILED);
			}
			else
			{
				if (r->m_type == 1)
				{
					Bit8u *p = r->m_tables[0].m_code_size; Bit32u i;
					r->m_table_sizes[0] = 288; r->m_table_sizes[1] = 32; TINFL_MEMSET(r->m_tables[1].m_code_size, 5, 32);
					for (i = 0; i <= 143; ++i) { *p++ = 8; } for (; i <= 255; ++i) { *p++ = 9; } for (; i <= 279; ++i) { *p++ = 7; } for (; i <= 287; ++i) { *p++ = 8; }
				}
				else
				{
					for (counter = 0; counter < 3; counter++) { TINFL_GET_BITS(TINFL_STATE_11, r->m_table_sizes[counter], "\05\05\04"[counter]); r->m_table_sizes[counter] += s_min_table_sizes[counter]; }
					TINFL_CLEAR(r->m_tables[2].m_code_size); for (counter = 0; counter < r->m_table_sizes[2]; counter++) { Bit32u s; TINFL_GET_BITS(TINFL_STATE_14, s, 3); r->m_tables[2].m_code_size[s_length_dezigzag[counter]] = (Bit8u)s; }
					r->m_table_sizes[2] = 19;
				}
				for ( ; (int)r->m_type >= 0; r->m_type--)
				{
					int tree_next, tree_cur; tinfl_huff_table *pTable;
					Bit32u i, j, used_syms, total, sym_index, next_code[17], total_syms[16]; pTable = &r->m_tables[r->m_type]; TINFL_CLEAR(total_syms); TINFL_CLEAR(pTable->m_look_up); TINFL_CLEAR(pTable->m_tree);
					for (i = 0; i < r->m_table_sizes[r->m_type]; ++i) total_syms[pTable->m_code_size[i]]++;
					used_syms = 0, total = 0; next_code[0] = next_code[1] = 0;
					for (i = 1; i <= 15; ++i) { used_syms += total_syms[i]; next_code[i + 1] = (total = ((total + total_syms[i]) << 1)); }
					if ((65536 != total) && (used_syms > 1))
					{
						TINFL_CR_RETURN_FOREVER(35, TINFL_STATUS_FAILED);
					}
					for (tree_next = -1, sym_index = 0; sym_index < r->m_table_sizes[r->m_type]; ++sym_index)
					{
						Bit32u rev_code = 0, l, cur_code, code_size = pTable->m_code_size[sym_index]; if (!code_size) continue;
						cur_code = next_code[code_size]++; for (l = code_size; l > 0; l--, cur_code >>= 1) rev_code = (rev_code << 1) | (cur_code & 1);
						if (code_size <= TINFL_FAST_LOOKUP_BITS) { Bit16s k = (Bit16s)((code_size << 9) | sym_index); while (rev_code < TINFL_FAST_LOOKUP_SIZE) { pTable->m_look_up[rev_code] = k; rev_code += (1 << code_size); } continue; }
						if (0 == (tree_cur = pTable->m_look_up[rev_code & (TINFL_FAST_LOOKUP_SIZE - 1)])) { pTable->m_look_up[rev_code & (TINFL_FAST_LOOKUP_SIZE - 1)] = (Bit16s)tree_next; tree_cur = tree_next; tree_next -= 2; }
						rev_code >>= (TINFL_FAST_LOOKUP_BITS - 1);
						for (j = code_size; j > (TINFL_FAST_LOOKUP_BITS + 1); j--)
						{
							tree_cur -= ((rev_code >>= 1) & 1);
							if (!pTable->m_tree[-tree_cur - 1]) { pTable->m_tree[-tree_cur - 1] = (Bit16s)tree_next; tree_cur = tree_next; tree_next -= 2; } else tree_cur = pTable->m_tree[-tree_cur - 1];
						}
						tree_cur -= ((rev_code >>= 1) & 1); pTable->m_tree[-tree_cur - 1] = (Bit16s)sym_index;
					}
					if (r->m_type == 2)
					{
						for (counter = 0; counter < (r->m_table_sizes[0] + r->m_table_sizes[1]); )
						{
							Bit32u s; TINFL_HUFF_DECODE(TINFL_STATE_16, dist, &r->m_tables[2]); if (dist < 16) { r->m_len_codes[counter++] = (Bit8u)dist; continue; }
							if ((dist == 16) && (!counter))
							{
								TINFL_CR_RETURN_FOREVER(17, TINFL_STATUS_FAILED);
							}
							num_extra = "\02\03\07"[dist - 16]; TINFL_GET_BITS(TINFL_STATE_18, s, num_extra); s += "\03\03\013"[dist - 16];
							TINFL_MEMSET(r->m_len_codes + counter, (dist == 16) ? r->m_len_codes[counter - 1] : 0, s); counter += s;
						}
						if ((r->m_table_sizes[0] + r->m_table_sizes[1]) != counter)
						{
							TINFL_CR_RETURN_FOREVER(21, TINFL_STATUS_FAILED);
						}
						TINFL_MEMCPY(r->m_tables[0].m_code_size, r->m_len_codes, r->m_table_sizes[0]); TINFL_MEMCPY(r->m_tables[1].m_code_size, r->m_len_codes + r->m_table_sizes[0], r->m_table_sizes[1]);
					}
				}
				for ( ; ; )
				{
					Bit8u *pSrc;
					for ( ; ; )
					{
						if (((pIn_buf_end_m_4 < pIn_buf_cur)) || ((pOut_buf_end_m_2 < pOut_buf_cur)))
						{
							TINFL_HUFF_DECODE(TINFL_STATE_23, counter, &r->m_tables[0]);
							if (counter >= 256)
								break;
							while (pOut_buf_cur >= pOut_buf_end) { TINFL_CR_RETURN(TINFL_STATE_24, TINFL_STATUS_HAS_MORE_OUTPUT); }
							*pOut_buf_cur++ = (Bit8u)counter;
						}
						else
						{
							int sym2; Bit32u code_len;
							#if MINIZ_HAS_64BIT_REGISTERS
							if (num_bits < 30) { bit_buf |= (((tinfl_bit_buf_t)ZIP_READ_LE32(pIn_buf_cur)) << num_bits); pIn_buf_cur += 4; num_bits += 32; }
							#else
							if (num_bits < 15) { bit_buf |= (((tinfl_bit_buf_t)ZIP_READ_LE16(pIn_buf_cur)) << num_bits); pIn_buf_cur += 2; num_bits += 16; }
							#endif

							sym2 = r_tables_0_look_up[bit_buf & (TINFL_FAST_LOOKUP_SIZE - 1)];
							if (sym2 < 0)
							{
								code_len = TINFL_FAST_LOOKUP_BITS;
								do { sym2 = r->m_tables[0].m_tree[~sym2 + ((bit_buf >> code_len++) & 1)]; } while (sym2 < 0);
							}
							else
								code_len = sym2 >> 9;
							counter = sym2;
							bit_buf >>= code_len;
							num_bits -= code_len;
							if (counter & 256)
								break;

							#if !MINIZ_HAS_64BIT_REGISTERS
							if (num_bits < 15) { bit_buf |= (((tinfl_bit_buf_t)ZIP_READ_LE16(pIn_buf_cur)) << num_bits); pIn_buf_cur += 2; num_bits += 16; }
							#endif

							sym2 = r_tables_0_look_up[bit_buf & (TINFL_FAST_LOOKUP_SIZE - 1)];
							if (sym2 >= 0)
								code_len = sym2 >> 9;
							else
							{
								code_len = TINFL_FAST_LOOKUP_BITS;
								do { sym2 = r->m_tables[0].m_tree[~sym2 + ((bit_buf >> code_len++) & 1)]; } while (sym2 < 0);
							}
							bit_buf >>= code_len;
							num_bits -= code_len;

							pOut_buf_cur[0] = (Bit8u)counter;
							if (sym2 & 256)
							{
								pOut_buf_cur++;
								counter = sym2;
								break;
							}
							pOut_buf_cur[1] = (Bit8u)sym2;
							pOut_buf_cur += 2;
						}
					}
					if ((counter &= 511) == 256) break;

					num_extra = s_length_extra[counter - 257]; counter = s_length_base[counter - 257];
					if (num_extra) { Bit32u extra_bits; TINFL_GET_BITS(TINFL_STATE_25, extra_bits, num_extra); counter += extra_bits; }

					TINFL_HUFF_DECODE(TINFL_STATE_26, dist, &r->m_tables[1]);
					num_extra = s_dist_extra[dist]; dist = s_dist_base[dist];
					if (num_extra) { Bit32u extra_bits; TINFL_GET_BITS(TINFL_STATE_27, extra_bits, num_extra); dist += extra_bits; }

					dist_from_out_buf_start = pOut_buf_cur - pOut_buf_start;
					if ((dist > dist_from_out_buf_start) && (decomp_flags & TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF))
					{
						TINFL_CR_RETURN_FOREVER(37, TINFL_STATUS_FAILED);
					}

					pSrc = pOut_buf_start + ((dist_from_out_buf_start - dist) & out_buf_size_mask);

					if ((ZIP_MAX(pOut_buf_cur, pSrc) + counter) <= pOut_buf_end)
					{
						do
						{
							pOut_buf_cur[0] = pSrc[0];
							pOut_buf_cur[1] = pSrc[1];
							pOut_buf_cur[2] = pSrc[2];
							pOut_buf_cur += 3; pSrc += 3;
						} while ((int)(counter -= 3) > 2);
						if ((int)counter > 0)
						{
							*(pOut_buf_cur++) = pSrc[0];
							if (counter == 2)
								*(pOut_buf_cur++) = pSrc[1];
						}
					}
					else
					{
						while (counter--)
						{
							while (pOut_buf_cur >= pOut_buf_end) { TINFL_CR_RETURN(TINFL_STATE_53, TINFL_STATUS_HAS_MORE_OUTPUT); }
							*pOut_buf_cur++ = pOut_buf_start[(dist_from_out_buf_start++ - dist) & out_buf_size_mask];
						}
					}
				}
			}
		} while (!(r->m_final & 1));
		TINFL_CR_RETURN_FOREVER(34, TINFL_STATUS_DONE);
		TINFL_CR_FINISH

		common_exit:
		r->m_num_bits = num_bits; r->m_bit_buf = bit_buf; r->m_dist = dist; r->m_counter = counter; r->m_num_extra = num_extra; r->m_dist_from_out_buf_start = dist_from_out_buf_start;
		*pIn_buf_size = (Bit32u)(pIn_buf_cur - pIn_buf_next); *pOut_buf_size = (Bit32u)(pOut_buf_cur - pOut_buf_next);
		return status;

		#undef TINFL_MACRO_END
		#undef TINFL_MEMCPY
		#undef TINFL_MEMSET
		#undef TINFL_CR_BEGIN
		#undef TINFL_CR_RETURN
		#undef TINFL_CR_RETURN_FOREVER
		#undef TINFL_CR_FINISH
		#undef TINFL_GET_BYTE
		#undef TINFL_NEED_BITS
		#undef TINFL_SKIP_BITS
		#undef TINFL_GET_BITS
		#undef TINFL_HUFF_BITBUF_FILL
		#undef TINFL_HUFF_DECODE
	}
};

struct oz_unshrink
{
	// BASED ON OZUNSHRINK
	// Ozunshrink / Old ZIP Unshrink (ozunshrink.h) (public domain)
	// By Jason Summers - https://github.com/jsummers/oldunzip

	enum
	{
		OZ_ERRCODE_OK                  = 0,
		OZ_ERRCODE_GENERIC_ERROR       = 1,
		OZ_ERRCODE_BAD_CDATA           = 2,
		OZ_ERRCODE_READ_FAILED         = 6,
		OZ_ERRCODE_WRITE_FAILED        = 7,
		OZ_ERRCODE_INSUFFICIENT_CDATA  = 8,
	};

	Bit8u *out_start, *out_cur, *out_end;
	Bit8u *in_start, *in_cur, *in_end;

	// The code table (implements a dictionary)
	enum { OZ_VALBUFSIZE = 7936, OZ_NUM_CODES = 8192 };
	Bit8u valbuf[OZ_VALBUFSIZE]; // Max possible chain length (8192 - 257 + 1 = 7936)
	struct { Bit16u parent; Bit8u value; Bit8u flags; } ct[OZ_NUM_CODES];

	static int Run(oz_unshrink *oz)
	{
		enum { OZ_INITIAL_CODE_SIZE = 9, OZ_MAX_CODE_SIZE = 13, OZ_INVALID_CODE = 256 };
		Bit32u oz_bitreader_buf = 0;
		Bit8u  oz_bitreader_nbits_in_buf = 0;
		Bit8u  oz_curr_code_size = OZ_INITIAL_CODE_SIZE;
		Bit16u oz_oldcode = 0;
		Bit16u oz_highest_code_ever_used = 0;
		Bit16u oz_free_code_search_start = 257;
		Bit8u  oz_last_value = 0;
		bool   oz_have_oldcode = false;
		bool   oz_was_clear = false;

		memset(oz->ct, 0, sizeof(oz->ct));
		for (Bit16u i = 0; i < 256; i++)
		{
			// For entries <=256, .parent is always set to OZ_INVALID_CODE.
			oz->ct[i].parent = OZ_INVALID_CODE;
			oz->ct[i].value = (Bit8u)i;
		}
		for (Bit16u j = 256; j < OZ_NUM_CODES; j++)
		{
			// For entries >256, .parent==OZ_INVALID_CODE means code is unused
			oz->ct[j].parent = OZ_INVALID_CODE;
		}

		for (;;)
		{
			while (oz_bitreader_nbits_in_buf < oz_curr_code_size)
			{
				if (oz->in_cur >= oz->in_end) return OZ_ERRCODE_INSUFFICIENT_CDATA;
				Bit8u b = *(oz->in_cur++);
				oz_bitreader_buf |= ((Bit32u)b) << oz_bitreader_nbits_in_buf;
				oz_bitreader_nbits_in_buf += 8;
			}

			Bit16u code = (Bit16u)(oz_bitreader_buf & ((1U << oz_curr_code_size) - 1U));
			oz_bitreader_buf >>= oz_curr_code_size;
			oz_bitreader_nbits_in_buf -= oz_curr_code_size;

			if (code == 256)
			{
				oz_was_clear = true;
				continue;
			}

			if (oz_was_clear)
			{
				oz_was_clear = false;

				if (code == 1 && (oz_curr_code_size < OZ_MAX_CODE_SIZE))
				{
					oz_curr_code_size++;
					continue;
				}
				if (code != 2) return OZ_ERRCODE_BAD_CDATA;

				// partial clear
				Bit16u i;
				for (i = 257; i <= oz_highest_code_ever_used; i++)
				{
					if (oz->ct[i].parent != OZ_INVALID_CODE)
					{
						oz->ct[oz->ct[i].parent].flags = 1; // Mark codes that have a child
					}
				}

				for (i = 257; i <= oz_highest_code_ever_used; i++)
				{
					if (oz->ct[i].flags == 0)
					{
						oz->ct[i].parent = OZ_INVALID_CODE; // Clear this code
						oz->ct[i].value = 0;
					}
					else
					{
						oz->ct[i].flags = 0; // Leave all flags at 0, for next time.
					}
				}

				oz_free_code_search_start = 257;
				continue;
			}

			// Process a single (nonspecial) LZW code that was read from the input stream.
			if (code >= OZ_NUM_CODES) return OZ_ERRCODE_GENERIC_ERROR;

			Bit16u emit_code;
			bool late_add, code_is_in_table = (code < 256 || oz->ct[code].parent != OZ_INVALID_CODE);
			if      (!oz_have_oldcode) { late_add = false; goto OZ_EMIT_CODE;         } //emit only
			else if (code_is_in_table) { late_add =  true; goto OZ_EMIT_CODE;         } //emit, then add
			else                       { late_add = false; goto OZ_ADD_TO_DICTIONARY; } //add, then emit

			// Add a code to the dictionary.
			OZ_ADD_TO_DICTIONARY:
			Bit16u newpos, valbuf_pos;
			for (newpos = oz_free_code_search_start; ; newpos++)
			{
				if (newpos >= OZ_NUM_CODES) return OZ_ERRCODE_BAD_CDATA;
				if (oz->ct[newpos].parent == OZ_INVALID_CODE) break;
			}
			oz->ct[newpos].parent = oz_oldcode;
			oz->ct[newpos].value = oz_last_value;
			oz_free_code_search_start = newpos + 1;
			if (newpos > oz_highest_code_ever_used)
			{
				oz_highest_code_ever_used = newpos;
			}
			if (late_add) goto OZ_FINISH_PROCESS_CODE;

			// Decode an LZW code to one or more values, and write the values. Updates oz_last_value.
			OZ_EMIT_CODE:
			for (emit_code = code, valbuf_pos = OZ_VALBUFSIZE;;) // = First entry that's used
			{
				if (emit_code >= OZ_NUM_CODES) return OZ_ERRCODE_GENERIC_ERROR;

				// Check if infinite loop (probably an internal error).
				if (valbuf_pos == 0) return OZ_ERRCODE_GENERIC_ERROR;

				// valbuf is a stack, essentially. We fill it in the reverse direction, to make it simpler to write the final byte sequence.
				valbuf_pos--;

				if (emit_code >= 257 && oz->ct[emit_code].parent == OZ_INVALID_CODE)
				{
					oz->valbuf[valbuf_pos] = oz_last_value;
					emit_code = oz_oldcode;
					continue;
				}

				oz->valbuf[valbuf_pos] = oz->ct[emit_code].value;

				if (emit_code < 257)
				{
					oz_last_value = oz->ct[emit_code].value;

					// Write out the collected values.
					size_t n = OZ_VALBUFSIZE - valbuf_pos;
					if (oz->out_cur + n > oz->out_end) return OZ_ERRCODE_WRITE_FAILED;
					memcpy(oz->out_cur, &oz->valbuf[valbuf_pos], n);
					oz->out_cur += n;
					if (oz->out_cur == oz->out_end) return OZ_ERRCODE_OK;

					break;
				}

				// Traverse the tree, back toward the root codes.
				emit_code = oz->ct[emit_code].parent;
			}
			if (late_add) goto OZ_ADD_TO_DICTIONARY;

			if (!oz_have_oldcode)
			{
				oz_have_oldcode = true;
				oz_last_value = (Bit8u)code;
			}

			OZ_FINISH_PROCESS_CODE:
			oz_oldcode = code;
		}
	}
};

struct unz_explode
{
	// BASED ON INFO-ZIP UNZIP
	// Info-ZIP UnZip v5.4 (explode.c and inflate.c)
	// Put in the public domain by Mark Adler

	enum
	{
		UNZ_ERRCODE_OK                  = 0,
		UNZ_ERRCODE_INCOMPLETE_SET      = 1,
		UNZ_ERRCODE_INVALID_TABLE_INPUT = 2,
		UNZ_ERRCODE_OUTOFMEMORY         = 3,
		UNZ_ERRCODE_INVALID_TREE_INPUT  = 4,
		UNZ_ERRCODE_INTERNAL_ERROR      = 5,
		UNZ_ERRCODE_OUTPUT_ERROR        = 6,
	};

	Bit8u *out_start, *out_cur, *out_end;
	Bit8u *in_start, *in_cur, *in_end;

	enum { WSIZE = 0x8000 }; // window size--must be a power of two
	Bit8u slide[WSIZE];

	static Bit8u GetByte(unz_explode* exploder)
	{
		return (exploder->in_cur < exploder->in_end ? *(exploder->in_cur++) : 0);
	}

	struct huft
	{
		// number of extra bits or operation, number of bits in this code or subcode
		Bit8u e, b;
		// literal, length base, or distance base || pointer to next level of table
		union { Bit16u n; huft *t; } v;
	};

	static void huft_free(huft *t)
	{
		for (huft *p = t, *q; p != (huft *)NULL; p = q)
		{
			q = (--p)->v.t;
			free(p);
		}
	}

	static int get_tree_build_huft(unz_explode* exploder, Bit32u *b, Bit32u n, Bit32u s, const Bit16u *d, const Bit16u *e, huft **t, int *m)
	{
		// Get the bit lengths for a code representation from the compressed stream.
		// If get_tree() returns 4, then there is an error in the data
		Bit32u bytes_remain;    // bytes remaining in list
		Bit32u lengths_entered; // lengths entered
		Bit32u ncodes;  // number of codes
		Bit32u bitlen; // bit length for those codes

		// get bit lengths
		bytes_remain = (Bit32u)GetByte(exploder) + 1; // length/count pairs to read
		lengths_entered = 0; // next code
		do
		{
			bitlen = ((ncodes = (Bit32u)GetByte(exploder)) & 0xf) + 1; //bits in code (1..16)
			ncodes = ((ncodes & 0xf0) >> 4) + 1; /* codes with those bits (1..16) */
			if (lengths_entered + ncodes > n) return UNZ_ERRCODE_INVALID_TREE_INPUT; // don't overflow bit_lengths
			do
			{
				b[lengths_entered++] = bitlen;
			} while (--ncodes);
		} while (--bytes_remain);
		if (lengths_entered != n) return UNZ_ERRCODE_INVALID_TREE_INPUT;

		// Mystery code, the original (huft_build function) wasn't much more readable IMHO (see inflate.c)
		// Given a list of code lengths and a maximum table size, make a set of tables to decode that set of codes.  Return zero on success, one if
		// the given code set is incomplete (the tables are still built in this case), two if the input is invalid (all zero length codes or an
		// oversubscribed set of lengths), and three if not enough memory.
		enum { BMAX = 16, N_MAX = 288 }; Bit32u a, c[BMAX + 1], f, i, j, *p, v[N_MAX], x[BMAX + 1], *xp, z; int g, h, k, l, w, y; huft *q, r, *u[BMAX];
		memset(c, 0, sizeof(c)); p = b; i = n; do { c[*p++]++; } while (--i); if (c[0] == n) { *t = (huft *)NULL; *m = 0; return UNZ_ERRCODE_OK; }
		l = *m; for (j = 1; j <= BMAX; j++) if (c[j]) break; k = j; if ((Bit32u)l < j) l = j; for (i = BMAX; i; i--) if (c[i]) break;
		g = i; if ((Bit32u)l > i) l = i; *m = l; for (y = 1 << j; j < i; j++, y <<= 1) if ((y -= c[j]) < 0) return UNZ_ERRCODE_INVALID_TABLE_INPUT;
		if ((y -= c[i]) < 0) { return UNZ_ERRCODE_INVALID_TABLE_INPUT; } c[i] += y; x[1] = j = 0; p = c + 1; xp = x + 2; while (--i) { *xp++ = (j += *p++); }
		p = b; i = 0; do { if ((j = *p++) != 0) v[x[j]++] = i; } while (++i < n); x[0] = i = 0; p = v; h = -1; w = -l;
		u[0] = (huft *)NULL; q = (huft *)NULL; z = 0; for (; k <= g; k++) { a = c[k]; while (a--) { while (k > w + l)
		{ h++; w += l; z = (z = g - w) > (Bit32u)l ? l : z; if ((f = 1 << (j = k - w)) > a + 1) { f -= a + 1; xp = c + k; while (++j < z)
		{ if ((f <<= 1) <= *++xp) break; f -= *xp; } } z = 1 << j; if ((q = (huft *)malloc((z + 1)*sizeof(huft))) == (huft *)NULL)
		{ if (h) huft_free(u[0]); return UNZ_ERRCODE_OUTOFMEMORY; } *t = q + 1; *(t = &(q->v.t)) = (huft *)NULL; u[h] = ++q; if (h)
		{ x[h] = i; r.b = (Bit8u)l; r.e = (Bit8u)(16 + j); r.v.t = q; j = i >> (w - l); u[h - 1][j] = r; } } r.b = (Bit8u)(k - w); if (p >= v + n) r.e = 99; else if (*p < s)
		{ r.e = (Bit8u)(*p < 256 ? 16 : 15); r.v.n = (Bit16u)*p++; } else
		{ r.e = (Bit8u)e[*p - s]; r.v.n = d[*p++ - s]; } f = 1 << (k - w); for (j = i >> w; j < z; j += f) q[j] = r; for (j = 1 << (k - 1);
		i & j; j >>= 1) { i ^= j; } i ^= j; while ((i & ((1 << w) - 1)) != x[h]) { h--; w -= l; } } }
		return (y == 0 || g == 1 ? UNZ_ERRCODE_OK : UNZ_ERRCODE_INCOMPLETE_SET);
	}

	static int flush(unz_explode* exploder, Bit32u w)
	{
		Bit8u *out_w = exploder->out_cur + w;
		int ret = (out_w > exploder->out_end ? 1 : 0);
		if (ret) out_w = exploder->out_end;
		memcpy(exploder->out_cur, exploder->slide, (out_w - exploder->out_cur));
		exploder->out_cur = out_w;
		return ret;
	}

	static int Run(unz_explode* exploder, Bit16u zip_bit_flag)
	{
		/* Tables for length and distance */
		static const Bit16u cplen2[]    = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65 };
		static const Bit16u cplen3[]    = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66 };
		static const Bit16u extra[]     = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8 };
		static const Bit16u cpdist4[]   = { 1, 65, 129, 193, 257, 321, 385, 449, 513, 577, 641, 705, 769, 833, 897, 961, 1025, 1089, 1153, 1217, 1281, 1345, 1409, 1473, 1537, 1601, 1665, 1729, 1793, 1857, 1921, 1985, 2049, 2113, 2177, 2241, 2305, 2369, 2433, 2497, 2561, 2625, 2689, 2753, 2817, 2881, 2945, 3009, 3073, 3137, 3201, 3265, 3329, 3393, 3457, 3521, 3585, 3649, 3713, 3777, 3841, 3905, 3969, 4033 };
		static const Bit16u cpdist8[]   = { 1, 129, 257, 385, 513, 641, 769, 897, 1025, 1153, 1281, 1409, 1537, 1665, 1793, 1921, 2049, 2177, 2305, 2433, 2561, 2689, 2817, 2945, 3073, 3201, 3329, 3457, 3585, 3713, 3841, 3969, 4097, 4225, 4353, 4481, 4609, 4737, 4865, 4993, 5121, 5249, 5377, 5505, 5633, 5761, 5889, 6017, 6145, 6273, 1, 6529, 6657, 6785, 6913, 7041, 7169, 7297, 7425, 7553, 7681, 7809, 7937, 8065 };
		static const Bit16u mask_bits[] = { 0x0000, 0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff, 0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff };

		huft *tb = NULL, *tl = NULL, *td = NULL; // literal code, length code, distance code tables
		Bit32u l[256]; // bit lengths for codes
		bool is8k  = ((zip_bit_flag & 2) == 2), islit = ((zip_bit_flag & 4) == 4);
		int bb = (islit ? 9 : 0), bl = 7, bd = ((exploder->in_end - exploder->in_start)  > 200000 ? 8 : 7); // bits for tb, tl, td
		Bit32u numbits = (is8k ? 7 : 6);

		int r;
		if (islit && (r = get_tree_build_huft(exploder, l, 256, 256, NULL, NULL, &tb, &bb)) != 0) goto done;
		if ((r = get_tree_build_huft(exploder, l, 64, 0, (islit ? cplen3 : cplen2), extra, &tl, &bl)) != 0) goto done;
		if ((r = get_tree_build_huft(exploder, l, 64, 0, (is8k ? cpdist8 : cpdist4), extra, &td, &bd)) != 0) goto done;

		// The implode algorithm uses a sliding 4K or 8K byte window on the uncompressed stream to find repeated byte strings.
		// This is implemented here as a circular buffer. The index is updated simply by incrementing and then and'ing with 0x0fff (4K-1) or 0x1fff (8K-1).
		// Here, the 32K buffer of inflate is used, and it works just as well to always have a 32K circular buffer, so the index is anded with 0x7fff.
		// This is done to allow the window to also be used as the output buffer.
		Bit32u s;          // bytes to decompress
		Bit32u e;          // table entry flag/number of extra bits
		Bit32u n, d;       // length and index for copy
		Bit32u w;          // current window position
		Bit32u mb, ml, md; // masks for bb (if lit), bl and bd bits
		Bit32u b;          // bit buffer
		Bit32u k;          // number of bits in bit buffer
		Bit32u u;          // true if unflushed
		huft *t;           // pointer to table entry

		#define UNZ_NEEDBITS(n) do {while(k<(n)){b|=((Bit32u)GetByte(exploder))<<k;k+=8;}} while(0)
		#define UNZ_DUMPBITS(n) do {b>>=(n);k-=(n);} while(0)

		// explode the coded data
		b = k = w = 0; // initialize bit buffer, window
		u = 1;         // buffer unflushed

		// precompute masks for speed
		mb = mask_bits[bb];
		ml = mask_bits[bl];
		md = mask_bits[bd];
		s = (Bit32u)(exploder->out_end - exploder->out_start);
		while (s > 0) // do until ucsize bytes uncompressed
		{
			UNZ_NEEDBITS(1);
			if (b & 1) // then literal
			{
				UNZ_DUMPBITS(1);
				s--;
				if (tb)
				{
					// LIT: Decompress the imploded data using coded literals and an 8K sliding window.
					UNZ_NEEDBITS((Bit32u)bb); // get coded literal
					if ((e = (t = tb + ((~(Bit32u)b) & mb))->e) > 16)
					{
						do
						{
							if (e == 99) { r = UNZ_ERRCODE_INTERNAL_ERROR; goto done; }
							UNZ_DUMPBITS(t->b);
							e -= 16;
							UNZ_NEEDBITS(e);
						} while ((e = (t = t->v.t + ((~(Bit32u)b) & mask_bits[e]))->e) > 16);
					}
					UNZ_DUMPBITS(t->b);
					exploder->slide[w++] = (Bit8u)t->v.n;
					if (w == WSIZE) { if (flush(exploder, w)) { r = UNZ_ERRCODE_OUTPUT_ERROR; goto done; } w = u = 0; }
				}
				else
				{
					// UNLIT: Decompress the imploded data using uncoded literals and an 8K sliding window.
					UNZ_NEEDBITS(8);
					exploder->slide[w++] = (Bit8u)b;
					if (w == WSIZE) { if (flush(exploder, w)) { r = UNZ_ERRCODE_OUTPUT_ERROR; goto done; } w = u = 0; }
					UNZ_DUMPBITS(8);
				}
			}
			else // else distance/length
			{
				UNZ_DUMPBITS(1);
				UNZ_NEEDBITS(numbits); // get distance low bits
				d = (Bit32u)b & ((1 << numbits) - 1);
				UNZ_DUMPBITS(numbits);
				UNZ_NEEDBITS((Bit32u)bd); // get coded distance high bits
				if ((e = (t = td + ((~(Bit32u)b) & md))->e) > 16)
				{
					do
					{
						if (e == 99) { r = UNZ_ERRCODE_INTERNAL_ERROR; goto done; }
						UNZ_DUMPBITS(t->b);
						e -= 16;
						UNZ_NEEDBITS(e);
					} while ((e = (t = t->v.t + ((~(Bit32u)b) & mask_bits[e]))->e) > 16);
				}
				UNZ_DUMPBITS(t->b);
				d = w - d - t->v.n; // construct offset
				UNZ_NEEDBITS((Bit32u)bl); // get coded length
				if ((e = (t = tl + ((~(Bit32u)b) & ml))->e) > 16)
				{
					do
					{
						if (e == 99) { r = UNZ_ERRCODE_INTERNAL_ERROR; goto done; }
						UNZ_DUMPBITS(t->b);
						e -= 16;
						UNZ_NEEDBITS(e);
					} while ((e = (t = t->v.t + ((~(Bit32u)b) & mask_bits[e]))->e) > 16);
				}
				UNZ_DUMPBITS(t->b);
				n = t->v.n;
				if (e) // get length extra bits
				{
					UNZ_NEEDBITS(8);
					n += (Bit32u)b & 0xff;
					UNZ_DUMPBITS(8);
				}

				// do the copy
				s -= n;
				do
				{
					n -= (e = (e = WSIZE - ((d &= WSIZE - 1) > w ? d : w)) > n ? n : e);
					if (u && w <= d)
					{
						memset(exploder->slide + w, 0, e);
						w += e;
						d += e;
					}
					else if (w - d >= e) // (this test assumes unsigned comparison)
					{
						memcpy(exploder->slide + w, exploder->slide + d, e);
						w += e;
						d += e;
					}
					else // do it slow to avoid memcpy() overlap
					{
						do {
							exploder->slide[w++] = exploder->slide[d++];
						} while (--e);
					}
					if (w == WSIZE)
					{
						if (flush(exploder, w)) { r = UNZ_ERRCODE_OUTPUT_ERROR; goto done; }
						w = u = 0;
					}
				} while (n);
			}
		}

		#undef UNZ_NEEDBITS
		#undef UNZ_DUMPBITS

		/* flush out slide */
		if (flush(exploder, w)) { r = UNZ_ERRCODE_OUTPUT_ERROR; goto done; }

		done:
		huft_free(td);
		huft_free(tl);
		huft_free(tb);
		return r;
	}
};

bool Zip_Archive::Unpack(Bit64u zf_data_ofs, Bit32u zf_comp_size, Bit32u zf_uncomp_size, Bit8u zf_bit_flags, Bit8u zf_method, std::vector<Bit8u>& mem_data)
{
	Bit8u local_header[ZIP_LOCAL_DIR_HEADER_SIZE];
	if (Read(zf_data_ofs, local_header, ZIP_LOCAL_DIR_HEADER_SIZE) != ZIP_LOCAL_DIR_HEADER_SIZE)
		return false;
	if (ZIP_READ_LE32(local_header) != ZIP_LOCAL_DIR_HEADER_SIG)
		return false;
	zf_data_ofs += ZIP_LOCAL_DIR_HEADER_SIZE + ZIP_READ_LE16(local_header + ZIP_LDH_FILENAME_LEN_OFS) + ZIP_READ_LE16(local_header + ZIP_LDH_EXTRA_LEN_OFS);
	if ((zf_data_ofs + zf_comp_size) > size)
		return false;

	mem_data.resize(zf_uncomp_size);
	if (!zf_uncomp_size) return true;
	else if (zf_method == METHOD_DEFLATED)
	{
		miniz::tinfl_decompressor inflator;
		Bit64u pos = zf_data_ofs;
		Bit32u out_buf_ofs = 0, read_buf_avail = 0, read_buf_ofs = 0, comp_remaining = zf_comp_size;
		Bit8u read_buf[miniz::MZ_ZIP_MAX_IO_BUF_SIZE], *out_data = &mem_data[0];
		miniz::tinfl_init(&inflator);
		for (miniz::tinfl_status status = miniz::TINFL_STATUS_NEEDS_MORE_INPUT; status == miniz::TINFL_STATUS_NEEDS_MORE_INPUT || status == miniz::TINFL_STATUS_HAS_MORE_OUTPUT;)
		{
			if (!read_buf_avail)
			{
				read_buf_avail = (comp_remaining < miniz::MZ_ZIP_MAX_IO_BUF_SIZE ? comp_remaining : miniz::MZ_ZIP_MAX_IO_BUF_SIZE);
				if (Read(pos, read_buf, read_buf_avail) != read_buf_avail)
					break;
				pos += read_buf_avail;
				comp_remaining -= read_buf_avail;
				read_buf_ofs = 0;
			}
			Bit32u out_buf_size = zf_uncomp_size - out_buf_ofs;
			Bit8u *pWrite_buf_cur = out_data + out_buf_ofs;
			Bit32u in_buf_size = read_buf_avail;
			status = miniz::tinfl_decompress(&inflator, read_buf + read_buf_ofs, &in_buf_size, out_data, pWrite_buf_cur, &out_buf_size, miniz::TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF | (comp_remaining ? miniz::TINFL_FLAG_HAS_MORE_INPUT : 0));
			read_buf_avail -= in_buf_size;
			read_buf_ofs += in_buf_size;
			out_buf_ofs += out_buf_size;
			ZIP_ASSERT(!out_buf_size || out_buf_ofs <= zf_uncomp_size);
			ZIP_ASSERT(status == miniz::TINFL_STATUS_NEEDS_MORE_INPUT || status == miniz::TINFL_STATUS_HAS_MORE_OUTPUT || status == miniz::TINFL_STATUS_DONE);
		}
	}
	else if (zf_method == METHOD_STORED)
	{
		Read(zf_data_ofs, &mem_data[0], zf_uncomp_size);
	}
	else if (zf_method == METHOD_SHRUNK)
	{
		oz_unshrink *unshrink = (oz_unshrink*)malloc(sizeof(oz_unshrink) + zf_comp_size);
		Bit8u* in_buf = (Bit8u*)(unshrink + 1);
		if (Read(zf_data_ofs, in_buf, zf_comp_size) == zf_comp_size)
		{
			mem_data.resize(zf_uncomp_size);
			unshrink->in_start = unshrink->in_cur = in_buf;
			unshrink->in_end = in_buf + zf_comp_size;
			unshrink->out_start = unshrink->out_cur = &mem_data[0];
			unshrink->out_end = unshrink->out_start + zf_uncomp_size;
			#ifndef NDEBUG
			int res =
			#endif
			oz_unshrink::Run(unshrink);
			ZIP_ASSERT(res == 0);
		}
		free(unshrink);
	}
	else if (zf_method == METHOD_IMPLODED)
	{
		unz_explode *explode = (unz_explode*)malloc(sizeof(unz_explode) + zf_comp_size);
		Bit8u* in_buf = (Bit8u*)(explode + 1);
		if (Read(zf_data_ofs, in_buf, zf_comp_size) == zf_comp_size)
		{
			mem_data.resize(zf_uncomp_size);
			explode->in_start = explode->in_cur = in_buf;
			explode->in_end = in_buf + zf_comp_size;
			explode->out_start = explode->out_cur = &mem_data[0];
			explode->out_end = explode->out_start + zf_uncomp_size;
			#ifndef NDEBUG
			int res =
			#endif
			unz_explode::Run(explode, zf_bit_flags);
			ZIP_ASSERT(res == 0);
		}
		free(explode);
	}
	else { mem_data.clear(); return false; }
	return true;
}
