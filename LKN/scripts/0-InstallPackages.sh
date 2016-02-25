#!/bin/bash

## @lkn{Samu} The following script is used to install the necessary software packages on Linux

sudo apt-get install python-dev
sudo apt-get install scons

# Simulation packages
sudo apt-get install python-pip
sudo apt-get install python-tk
sudo pip install bottle
sudo pip install PyDispatcher

# Compiler for MSP430
sudo apt-get install gcc-msp430

