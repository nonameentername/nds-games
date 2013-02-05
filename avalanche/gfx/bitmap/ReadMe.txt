gfx2gba
=======

Version 1.03 - Tuesday, November 28, 2001

http://www.gbacode.net

Introduction
============

gfx2gba is a command line utility that converts many different common
image formats to formats suitable for the GameBoy Advance.
Output can be in a raw binary format or a C style header file.

Supported input formats are:
	BMP, ICO, IFF, JNG, JPEG/JIF, KOALA, Kodak PhotoCD, LBM, MNG
	PCX, PBM, PGM, PNG, PPM, PhotoShop, Sun RAS, TARGA, TIFF and WBMP

Supported output formats are:
	8 bit (with palette) and 15 bit BGR format data
	8 bit (with palette) GBA sprite/tile

Questions about gfx2gba should be directed to <darren@gbacode.net> or in
the forums available at http://forums.gbacode.net

Distribution
============

The latest distribution will always be available at http://www.gbacode.net

Usage
=====

The command line utility has the following usage:
    gfx2gba infile outfile [-15]|[-8] [-v] [-w n] [-s n] [-n]
                           [-c] [-t type] [-r]|[-o] [-k] [-z n]
						   [-q type] [-b] [-p file] [-m]

The options have the following meaning:
    -15      15 bit, convert to 15 bit BGR format, default
    -8       8 bit, convert to 8 bit format
    -v       verbose
    -w n     width, number of entries of data array per line, default = 8
    -s n     size, size in bytes of each entry, must be 1, 2 or 4, default = 1
	-n name  name, C variable name
    -c       C types, use standard C types instead of common GBA types
    -t type  type, type of data array, default depends on the following:
                 -s 1   => u8;   with -c => unsigned char
                 -s 2   => u16;  with -c => unsigned short int
                 -s 4   => u32;  with -c => unsigned int
    -r       RGB cube, create standardized RGB cube 8 bit palette, default
    -o       optimal, create optimal 8 bit palette
	-k       no const, do not use the const modifier for type of data array
    -z n     sprite, output in GBA sprite/tile format
                 -z 256 => 256 colors x 1 palette, default
    -q type  quantizer, specify quantizer to create optimal 8 bit palette
                 -q wu  => Wu's quantizer, default
                 -q nn  => neural net quantizer
    -b       binary, write image data out to a binary file
	-p file  palette, write palette data out to a binary file
	-m       merge, merge palette data, use with -p option

Notes
=====

The -p and -m options are used to manipulate palettes. For simple use, the
-p option allows the user to output the palette data for an 8 bit image as
a separate 512 byte file. 

The -m option can be used to create a single palette across multiple images.
To do this, the file specified by the -p option is used as a base palette
for the image being converted. Colors unique to the image (that do not exist
in the base palette) are added to the palette, which is then output.

For example, to use this functionality in a makefile to create a single
palette across all your sprite bitmaps you would use a rule similar to the
following:

	%.raw : %.bmp
		gfx2gba.exe $< $@ -b -z 256 -o -p combined.pal -m

For the -p option, the palette file is write only; for the -m option, the
palette file is read/write.

There is the possibility that when merging the palettes, more than 256
unique colors are needed. In this instance, the palette is truncated at 256
entries and the image data will contain some incorrect color data.

Changes
=======

1.03 (November 28, 2001)
  * Added -p option to output palette data to a separate binary file
  * Added -m option to allow merging of palette data

1.02 (November 12, 2001)
  * Fixed bug in 256 color GBA sprite format when using optimal palette
  * Updated to FreeImage v2.4.2

1.01 (September 3, 2001)
  * Updated to FreeImage v2.4.1 - now includes support for reading
    PhotoShop files
  * Added support for 256 color GBA sprite format
  * Changed default to add const modifier and added -k option to
    allow original behavior
  * Added -q option for specifying color quantizer
  * Added -n option for specifying C variable name

Warranty
========

This tool is provided for free but without warranty of any kind.

Credits
=======

gfx2gba uses the FreeImage image library (v2.4.2).
FreeImage is an open source image library supporting all common
bitmap formats. See http://www.6ixsoft.com for more details.

Copyright
=========

Please see the individual copyright notice in license-fi.txt.

Darren Sillett <darren@gbacode.net>
