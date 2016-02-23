#!/usr/bin/python

import sys, os, re

from subprocess import Popen, PIPE


if len(sys.argv)!=3:
	exit('need more arguments')

min_range = int(sys.argv[1])
max_range = int(sys.argv[2])


(devs, stderr) = Popen(['ls','/dev/'], stdout=PIPE).communicate()
#print(devs)

usbs = re.findall("(ttyUSB.)", devs)
print(usbs)

"""
for dev in devs:
	print(dev)
	if not (re.search("ttyUSB.",dev)==None):
		print('match!')
		usbs.append(dev)
"""
if (max_range-min_range+1)!=(len(usbs)-3):
	exit('range does not match')

#os.system('sed -i "20s/.*/ #define SLOTFRAME_LENGTH    %d/" ./fw-edit/schedule.h'%(max_range-min_range+1+2))

i=0
for usb in usbs:

	if usb=='ttyUSB0' or usb=='ttyUSB1' or usb=='ttyUSB2':
		continue

	if min_range==1 and i==0:
		os.system('sed -i "21s/.*/#define DAGROOT 1/" ./fw-edit/opendefs.h')
	else:
		os.system('sed -i "21s/.*/ /" ./fw-edit/opendefs.h')

	os.system('sed -i "60s/.*/   idmanager_vars.my64bID.addr_64b[7]   = %d;/" ./fw-edit/idmanager.c'%(min_range+i))
	os.system('./applyApp.sh')
	os.system('./applyEdits.sh')
	#	print(usb)
	os.system('./BuildUploadHW.sh /dev/' + usb)
	i += 1

