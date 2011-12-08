#!/bin/bash 

git rm autogen.sh ChangeLog config.h.in configure.ac Makefile.am README stamp-h.in 
git rm -r bindings
git rm -r cern_stuff cern_utils
git rm -r cmt
git rm -r dao
git rm -r flat
git rm -r imagemagick_utils
git rm -r m4
git rm -r mc
git rm -r src
git rm -r src_base
git rm -r src_utils
git rm -r lapack_stuff
git rm -r telinst 
git rm -r psf 
git rm -r lc 
git rm -r utils

mkdir tools
git add tools

# simphot 
git mv simphot/mklc.cc tools
git mv simphot/testlc.cc tools
git mv simphot/testpsf.cc tools
git rm simphot/array2d.h simphot/intframe.* simphot/intpoint.h simphot/pixelblock.*  simphot/Makefile.am
git mv simphot src 
