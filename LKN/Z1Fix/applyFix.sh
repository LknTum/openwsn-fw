#!/bin/bash

mkdir ../../bootloader/z1
cp z1-bsl-nopic ../../bootloader/z1
cp SConscript ../..
cp moteProbe.py ../../../openwsn-sw/software/openvisualizer/openvisualizer/moteProbe
cd ..
