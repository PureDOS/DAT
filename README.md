# Pure DOS DAT
This repository contains a ROM management data file (datfile in [XML format](http://www.logiqx.com/Dats/datafile.dtd))
of verified DOS games that can be easily loaded in a supported DOS emulator.

In addition, the repository contains the game specific [DOSC](#dosc-files) and [DOS.YML files](#dosyml) as listed
in the DAT file which simplify the launching of DOS games in various ways. See the respective sections for details.

The actual game files as stored in [DOSZ files](#dosz-files) are not contained in this repository.

For adding a new game to the DAT file, see [Contributing](#contributing) for how to decide on the file set.

## PureDOSDAT.xml
This is the DAT file which can be loaded in a ROM management tool to verify and build DOSZ and DOSC files.

[DoDAT](https://github.com/schellingb/DoDAT) is recommended because it has been optimized for this project and builds
correct ZIP files which keep the date and time information contained in the DAT file intact. DoDAT can automatically
source files from inside CD-ROM and floppy images even if those themselves are contained in a container like a ZIP file.

To use other ROM management tools, see the section on [RomVault](#romvault)

### PureDOSDAT-Variants.xml
Similar to the primary file this contains secondary variants of game packages which includes regional variants as well as original installation media.

## DOSZ Files
A DOSZ file is a ZIP file which contains one DOS game that can be loaded in a DOS emulator similar to how a console game ROM from that era is used with console emulators.

## DOSC Files
A DOSC file is also a ZIP file but it contains config files created from a setup program separate from the games installation
as well as binary patches (in IPS/BPS/VCDIFF/[XOR](../../../MakeXORPatch) format) that remove copy protection schemes.  
DOSC files can also include launch configuration(s) using the features described below.

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

## Parent/Child DOSZ Files
A DOSZ file can set itself to be a child of another DOSZ file by including an empty file with a .dosz.parent extension.  
For example, if a file named 'game.dosz.parent' is placed in a DOSZ, it will be loaded as an overlay over game.dosz.  
There can only be one .dosz.parent file but a parent can be a child of another parent (as long as there is no circular dependency).

The DOSC for a child DOSZ can override files contained in a parent (DOSZ or DOSC).
But a binary patch can only apply to a file that exists in the DOSZ with the same name as the DOSC.

## Shared Saves
If a DOSZ file contains an empty file with a .savename extension, modifications to the C: drive will
be stored in a save file named after that file instead of the DOSZ file itself.

For example, if parent and child DOSZ files each contain a .savename file with the same name,
save data like configurations, options or game saves will be shared between them.

Another example is for a game series in which a sequel can import save data from a predecessor. In such a case the
DOSZ files of each entry of the series should be installed in a directory not directly in the root of the C: drive.

## DOSC Launch Configurations
A DOSC file can specify more than one launch configuration by including a directory enclosed in [ ] brackets like `[My Config]`.  
A supported emulator will then on startup ask the user which launch configuration (or the default) should be used.

A launch configuration can do anything the root of the DOSC file can (file overrides, file patches and a DOS.YML file). As a minimum
it should contain a DOS.YML file with an auto start setting (either a `run_path` or `run_boot` key).

### Launch Config Categories
If a launch configuration name contains one or more `+` signs, the user can be prompted via a multiple choice menu.  
For example, if there are 3 directories `[Adlib + German]`, `[MIDI + English] and `[MIDI + German]`, the user will be prompted with
two categories where one has options "Adlib" and "MIDI" and the other "English" and "German". The emulator will automatically use
the default launch configuration (as defined in the root of the DOSC file) for the one combination that does not exist as an explicit
directory (in the example that would be "Adlib" and "English").

Additionally, a DOSC can contain a combination of categories and regular launch configurations which could be additional tools for a game.

### Launch Config Display Order
By default the launch configurations will be offered in alphabetical order. When using categories, the default combination will always show
at the top of the multiple choice menu of each category. If required, a custom order can be defined by prefixing an item with `n#` or `nn#`
where n/nn is a number. For example `[3#Setup]` or `[2#Adlib + 3#CGA]`. Such a prefix will then be hidden in the user interface.

## Loading Order
The number of file system overlays can become quite complex when using all the features above.  
Generally the order is parent-dosz → parent-dosc → child1-dosz → child1-dosc → child2-dosz → child2-dosc ... and finally the launch config directory (or directores).

Here's an example with multiple layers of parent DOSZ files and launch configurations in each layer where there is a main game package with
an expansion that depends on it and a second expansion that depends on the first one. On top of that each defines a [MIDI] launch config.
This is the order how all the packages will be overlaid in the file system and how DOS.YML files will be loaded:

- `Game Main.dosz`
- `Game Main.dosc`
- `Game Expansion 1.dosz` (contains `Game Main.dosz.parent`)
- `Game Expansion 1.dosc`
- `Game Expansion 2.dosz` (contains `Game Expansion 1.dosz`)
- `Game Expansion 2.dosc`
- `Game Main.dosc/[MIDI]`
- `Game Expansion 1.dosc/[MIDI]`
- `Game Expansion 2.dosc/[MIDI]`

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

For CD-ROMs with audio tracks, the audio tracks can be compressed by using the [CHDtoOGG utility](../../../CHDtoOGG).

#### Floppy Disk Images
Floppy disk images should be included in the .DOSZ file in uncompressed raw disk IMG format.

## Generating DAT from ZIPs
To generate the data file XML elements you can use the [MakePureDOSDAT utility](../../../MakePureDOSDAT).

## RomVault
[RomVault](https://www.romvault.com/) can optionally be used to build DOSZ files but they will not be fully correct because date and time information will be missing.
Depending on your Windows or .NET installation, you might want to use [version 3.2.5](https://www.romvault.com/download/ROMVault_V3.2.5.zip) or the latest available.

While working with a ROM manager other than the recommended DoDAT, the files will need to be named .DOSZ.ZIP.
To do this easily in Windows, create a file named `_swap_zip_extension.bat` with the content:
```bat
IF EXIST *.DOSZ ( ren *.dosz *.dosz.zip ) ELSE ( ren *.dosz.zip *. )
```

<details>
  <summary>Alternatively as a shell script:</summary>

  ```sh
  #!/bin/sh

  if ls -- *.dosz > /dev/null 2>&1
  then
      for file in *.dosz
      do mv "$file" "$file".zip
      done
  else
      for file in *.zip
      do rename ".zip" "" "$file"
      done
  fi
  ```
</details>
