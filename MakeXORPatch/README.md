# Make XOR Patch
This is a tool to generate XOR patches for use in [DOSC Files](../#dosc-files).

## XOR Patch
An XOR patch describes the difference between two files and can be used to transform a base file
stored in a DOSZ file to another file. Only the XOR patch needs to be stored in the DOSC file.

## Usage
The tool is used in the command prompt with the following argument(s):

```sh
  Create XOR Patch:   MakeXORPatch <BASE FILE> <TARGET FILE> <OUTPUT .XOR PATCH FILE>

  Apply XOR Patch:    MakeXORPatch <.XOR PATCH FILE> <OUTPUT TARGET FILE>

  Get XOR Patch info: MakeXORPatch <.XOR PATCH FILE>
```

## Compiling
A precompiled EXE file for Windows can be found next to this README file.

For other platforms use either `./build-gcc.sh` or `build-clang.sh` to compile the tool for your system.
