# Make Pure DOS DAT
This is a simple tool to generate data file XML entries from DOSZ files.

## Usage
Just launch the tool in the command prompt with one or more DOSZ files as its argument(s):
```sh
MakePureDOSDAT PathToFile.dosz PathToAnotherFile.dosz
```

Or to export the output to a file instead of writing it to the console:
```sh
MakePureDOSDAT PathToFile.dosz PathToAnotherFile.dosz >MyPureDOSDAT.xml
```

## Compiling
A precompiled EXE file for Windows can be found next to this README file.

For other platforms use either `./build-gcc.sh` or `build-clang.sh` to compile the tool for your system.
