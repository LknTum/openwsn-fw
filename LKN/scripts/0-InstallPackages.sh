#!/bin/bash

## @lkn{Samu} The following script is used to install the necessary software packages on Linux

sudo apt-get -y install python-dev
sudo apt-get -y install scons

# Simulation packages
sudo apt-get -y install python-pip
sudo apt-get -y install python-tk
sudo pip -y install bottle
sudo pip -y install PyDispatcher

# Compiler for MSP430
sudo apt-get -y install gcc-msp430

