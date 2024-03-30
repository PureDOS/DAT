# Pure DOS DAT
This repository contains a ROM management data file (datfile in [XML format](http://www.logiqx.com/Dats/datafile.dtd))
of verified DOS games that can be easily loaded in a supported DOS emulator.

In addition, the repository contains the game specific [DOSC](#dosc-files) and [DOS.YML files](#dosyml) as listed
in the DAT file which simplify the launching of DOS games in various ways. See the respective sections for details.

The actual game files as stored in [DOSZ files](#dosz-files) are not contained in this repository.

For adding a new game to the DAT file, see [Contributing](#contributing) for how to decide on the file set.

## PureDOSDAT.xml
This is the DAT file which can be loaded in a ROM management tool to verify and build DOSZ and DOSC files.

[RomVault](https://www.romvault.com/) is recommended because it is easy to use and by default builds correct ZIP files.
Depending on your Windows or .NET installation, you might want to use [version 3.2.5](https://www.romvault.com/download/ROMVault_V3.2.5.zip) or the latest available.

While working with a ROM manager, the files will need to be named .DOSZ.ZIP and .DOSC.ZIP.
To do this easily in Windows, create a file _swap_zip_extension.bat with the content:
```bat
IF EXIST *.DOSZ ( ren *.dosz *.dosz.zip & ren *.dosc *.dosc.zip ) ELSE ( ren *.dosz.zip *. & ren *.dosc.zip *. )
```

### PureDOSDAT-Variants.xml
Similar to the primary file this contains secondary variants of game packages which includes regional variants as well as original installation media.

## DOSZ Files
A DOSZ file is a ZIP file which contains one DOS game that can be loaded in a DOS emulator similar to how a console game ROM from that era is used with console emulators.

## DOSC Files
A DOSC file is also a ZIP file but it contains config files created from a setup program separate from the games installation
as well as binary patches (in IPS/BPS/VCDIFF format) that remove copy protection schemes.

An existing DOSC file is always loaded as a file system overlay for a DOSZ file with a matching name.

You can find all official DOSC files in the [dosc directory](dosc).

## DOS.YML
A DOS.YML file describes hardware and software configuration for the DOS emulator to make running a game correctly as seamless as possible.

The format description can be found in the [yml README](yml/README.md).
The DOS.YML files referenced by the DAT file can be found in the [yml directory](yml).

Such a file can be bundled as part of the DOSZ or DOSC file of a game. If the game inherently does not run on a DOS emulator's
default compatibility mode it should be part of the core DOSZ file. Otherwise, if the game's configurable settings stored in
the DOSC file warrants special emulation settings, it would go in there. If the file exists in both archives, an emulator will
load the one in DOSZ first followed by the one in DOSC, which can then add or overwrite keys.

## File Naming
The format of each file used by the DAT is the following:
```
GAMENAME (YEAR) (COMPANY) (VARIANT)
```

GAMENAME follows chapter 2 of the [naming convention by No-Intro](https://datomatic.no-intro.org/stuff/The%20Official%20No-Intro%20Convention%20(20071030).pdf).
It includes rules regarding allowed characters, capitalization, subtitles, punctuation and more.

YEAR is simply the first year of the game's public release.

COMPANY is either the developer or the publisher, whichever is more relevant or prevalent. The company name does not follow any specific naming convention,
it should be written as it was written in the game or on its box. Company name suffixes like "Incorporated"/"Inc.", "Limited"/"Ltd." or "Corporation"/"Corp." are removed.

VARIANT should only be used for optional secondary variants of a game and there should always exist one DAT entry without the variant specifier.
One variant is "Installation", which is for the installation media which can be used to produce the primary variant but otherwise after installation isn't used anymore.
It can also contain a region name for a regionally limited or translated release.

One example of a full name with 2 variant specifiers would be:

> Monkey Island 2 - LeChuck's Revenge (1992) (Lucasfilm Games) (Installation, Germany).dosz

A package like this would then contain the floppy disk images of the German version of Monkey Island 2.

## Contributing
If you want to contribute a new game to the DAT file, you first need to decide on the fileset. DOS games often exist in many
variants with minor or major differences between them.

### Language
The primary variant (without regional variant specifier) should be the English version of a game.
If no official English version exists, the original language will be used.
If there is a common multilingual version available which includes English, it will be preferred.

### Installation
A game that is installed to a harddrive ideally is just the result of the finished untouched installation made from (near) original media.

The game files should exist in the top root directory of the resulting zipped file. In a DOS emulator, this will appear as the root of the C: drive.
Only if a game does not allow being run from the root of the C: drive, it should be put into a sub-directory.

### Version Patches
If there exists patches or patched versions, generally the most up-to-date version is desired. But if a patch has no meaningful additions
a previous version can be used, especially if it is more common. If a game has been released into the public domain or is currently available
for purchase on digital distribution, that fileset is preferred to be included in the DAT, though only if the set of files was available in
an original DOS release. Modern patches or updates made long after the original DOS release should not be included.

### Original Media Images
One or more image files can be included in the .DOSZ file.
Image files should be given a short name. For a set of multiples, the disc/disk number should always be included and start at 1, i.e. "Game Name - Disc 1.chd".

If a CD-ROM or floppy disk does not need to be inserted in an emulated drive to play an installed game,
it should not be included in the primary variant but instead be in a separate "Installation" variant.

#### CD-ROM Images
CD-ROM images should be included in the .DOSZ file in uncompressed, version 5 CHD format (created with `chdman.exe createcd -c none`).
This is to avoid double compression by both CHD and ZIP, which would be a detriment for load time and performance, while keeping a full game in a single .DOSZ file.

#### Floppy Disk Images
Floppy disk images should be included in the .DOSZ file in uncompressed raw disk IMG format.

## Generating DAT from ZIPs
To generate the data file XML elements you can use the bundled [MakePureDOSDAT utility](MakePureDOSDAT).
