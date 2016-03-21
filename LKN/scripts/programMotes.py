#!/usr/bin/python
import sys, os, re
from subprocess import Popen, PIPE
from programMote import program


def program_all(min_range, max_range):
	"""
    Program a range of motes
	"""
	(devs, stderr) = Popen(['ls','/dev/'], stdout=PIPE).communicate()

	usbs = re.findall("(ttyUSB.)", devs.decode("utf-8"))
	print(usbs)

	if (max_range-min_range+1)!=len(usbs):
		exit('range does not match')

	for idx, usb in enumerate(usbs):

		# mote with id 1
		if min_range == 1 and idx == 0:
			program('/dev/'+usb, min_range+idx, 0)
		else:
			program('/dev/'+usb, min_range+idx, 0)




if __name__=='__main__':


	if len(sys.argv)<2:
		exit('Usage: %s START_ID [END_ID]' % sys.argv[0])
	elif len(sys.argv)==2:
		min_range = int(sys.argv[1])
		max_range = int(sys.argv[1])
	else:
		min_range = int(sys.argv[1])
		max_range = int(sys.argv[2])

	os.system("python schedule.py")
	program_all(min_range, max_range)
