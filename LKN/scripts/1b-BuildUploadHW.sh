#!/bin/bash

# Building and uploading for the Z1 board

cd ../..


sudo scons board=z1 toolchain=mspgcc bootload=$1 oos_openwsn

#sudo scons board=z1 toolchain=mspgcc bootload=/dev/ttyUSB1 oos_openwsn
#sudo scons board=z1 toolchain=mspgcc bootload=/dev/ttyS0 verbose=1 bsp_uart

