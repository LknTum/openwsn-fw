#!/bin/bash

# Run the web interface

cd ../../../openwsn-sw/software/openvisualizer/

# Enable IPv6 in Linux
sudo sysctl -w net.ipv6.conf.all.disable_ipv6=0

# Web-based GUI
sudo scons runweb

# Command line GUI
#sudo scons rungui


