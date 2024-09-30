
# Harfbuzz code files

The CPP files are located in `harfbuzz/src` folder. 

Only two CPP files must be compiled:
- `harfbuzz.cc` is a wrapper over the other harfbuzz code files.
- `hb-alloc.c` provides the memory allocation functions over a BESTFIT_ALLOCATOR.

The include files are located in `harfbuzz/inc` folder.

# Howto Build harfbuzz.a Library

The harfbuzz CPP code can not be compiled by IAR Embedded Workbench. 
An alternative is to build a `harfbuzz.a` library file with the Arm GnuToolChain and to add this library to the IAR project.

Notes:

- The `harfbuzz.a` library is built by a makefile. 
- The Arm GnuToolChain must be added to the PATH environment variable.
- The build needs Freetype's include files. These files are located by default in `../../freetype/inc` but then can be provided to the makefile with the `FREETYPE_INCLUDE_DIR` variable.

`make harfbuzz.a FREETYPE_INCLUDE_DIR=<path_to_freetype_include_dir>`

An IAR Embedded Workbench project (`harfbuzz_lib.ewp`) is available to build the `harfbuzz.a` library from IAR. 
This project has a prebuild action command that executes the Arm GnuToolChain makefile.
This project can be included in the IAR workspace.

# GCC object files

The compiled `harfbuzz.o` refers to some LIBC and LIBGCC functions.
These functions will be missing if the `harfbuzz.a` library is used with another compiler.
Some LIBC and LIBGCC object files are automatically extracted from `libc.a` and `libgcc.a` and added to the `harfbuzz.a` archive file to bypass this issue.

---
_Copyright 2022 MicroEJ Corp. This file has been modified and/or created by MicroEJ Corp._    

_ This is part of HarfBuzz, a text shaping library._  

_Permission is hereby granted, without written agreement and without_  
_license or royalty fees, to use, copy, modify, and distribute this_  
_software and its documentation for any purpose, provided that the_  
_above copyright notice and the following two paragraphs appear in_  
_all copies of this software._  

_IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR_  
_DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES_  
_ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN_  
_IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH_  
_DAMAGE._  

_THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,_  
_BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND_  
_FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS_  
_ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO_  
_PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS._  