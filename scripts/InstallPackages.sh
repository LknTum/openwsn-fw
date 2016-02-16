#!/bin/bash

sudo apt-get install git

mkdir openwsn
cd openwsn/
git clone https://github.com/openwsn-berkeley/openwsn-fw.git
git clone https://github.com/openwsn-berkeley/openwsn-sw.git
git clone https://github.com/openwsn-berkeley/coap.git

sudo apt-get install python-dev
sudo apt-get install scons

#to start a simulation
sudo apt-get install python-pip
sudo apt-get install python-tk
sudo pip install bottle
sudo pip install PyDispatcher

#building and uploading for a specific board
sudo apt-get install gcc-msp430

