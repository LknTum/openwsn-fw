#!/bin/bash

cd ../..

# Build for python
scons board=python toolchain=gcc oos_openwsn

# Run the web interface

# Enable IPv6 in Linux
sudo sysctl -w net.ipv6.conf.all.disable_ipv6=0

cd ../openwsn-sw/software/openvisualizer/

# Web-based GUI
sudo scons runweb --sim --simCount=3

# Command line GUI
#sudo scons rungui --sim --simCount=2 #cannot setup direct channel
