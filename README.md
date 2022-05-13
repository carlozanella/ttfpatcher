# ttfpatcher

This program is inspired by TTFPatch https://github.com/rmuch/ttfpatch by Wolfram Eßer, for modifying the font embedding licensing flag of TTF fonts.

It does essentially the same thing but is much shorter by using C structs to parse the binary files. Endianness is properly taken care of. I have written this program solely for my own pleasure.

## Usage
./ttfpatcher file.ttf (newFsType),
where newFsType=0 means no licensing restrictions and newFsType=2 means embedding is restricted.

## Disclaimer
This code is for educational purposes only, and it shall not be used for intellectual property rights infringement. It is the user's responsibility to verify that he/she has the rights to modify the font file.
