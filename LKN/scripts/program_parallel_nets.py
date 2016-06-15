#!/usr/bin/python
import sys, os, re
from subprocess import Popen, PIPE
from programMote import program
from os.path import isfile, join


def program_network(ids, usbs):
	"""
    Program a 2-mote network
	"""
	
	program('/dev/' + usbs[0], ids[0], 0)
	program('/dev/' + usbs[1], ids[1], 0)


def program_all_networks(num_networks, schedule_folder):

	# check if we have enough usbs
	(devs, stderr) = Popen(['ls','/dev/'], stdout=PIPE).communicate()

	usbs = re.findall("(ttyUSB.)", devs.decode("utf-8"))
	print(usbs)

	if len(usbs) != num_networks*2+1:
		exit("USB number mismatch!")

	# assume two nodes per network
	schedule_files = [f for f in os.listdir(schedule_folder) if (isfile(join(schedule_folder, f)) and "json" in f)]

	print("Schedule files: ")
	print(schedule_files)

	for n in range(num_networks):
		# find the schedule file
		schedule_file = None
		for f in schedule_files:
			if f.startswith("%d-" % (n+1)):

				schedule_file = schedule_folder+"/"+f
				break

		if schedule_file is None:
			exit("No schedule file for the network found (schedule files should start with \"NETWORK_NUMBER-\")")

		# program the network
		sender_id = 2*(n+1)-1
		receiver_id = 2*(n+1)

		os.system("python schedule.py "+schedule_file+" ../../openstack/02b-MAChigh/static_schedule.h"+" ../../openstack/02b-MAChigh/schedule.h"+" ../../openstack/02a-MAClow/IEEE802154E.h"+" ../../openapps/uinject/uinject.h")

		program_network([sender_id, receiver_id], usbs[sender_id-1:sender_id+1])

	programMote('/dev/'+usbs[-1], 2*num_networks+1, 0)



if __name__=='__main__':
	
	if len(sys.argv)!=3:
		exit('Usage: %s NUM_NETWORKS SCHEDULE_FOLDER' % sys.argv[0])

	num_networks = int(sys.argv[1])
	schedule_folder = str(sys.argv[2])

	program_all_networks(num_networks, schedule_folder)
	
