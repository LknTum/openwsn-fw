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

def program_white_networks(num_networks, schedule_folder, rx=1	):

	# check if we have enough usbs
	(devs, stderr) = Popen(['ls','/dev/'], stdout=PIPE).communicate()

	usbs = re.findall("(ttyUSB.+)", devs.decode("utf-8"))
	print(usbs)

	if rx:
		if len(usbs) != num_networks+1:
			exit("USB number mismatch!")
	else:
		if len(usbs) != num_networks*4:
			exit("USB number mismatch!")

	# assume two nodes per network
	schedule_files = [f for f in os.listdir(schedule_folder) if (isfile(join(schedule_folder, f)) and ("json" in f) and ("~" not in f))]

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


		os.system("python schedule.py "+schedule_file+" ../../openstack/02b-MAChigh/static_schedule.h"+" ../../openstack/02b-MAChigh/schedule.h"+" ../../openstack/02a-MAClow/IEEE802154E.h"+" ../../openapps/uinject/uinject.h")

		# program the network
		if rx:
			# sender_id = 2*(n+1)-1
			receiver_id = 5*n+5
			program('/dev/' + usbs[n], receiver_id, 0)
		else:
			for i in range(1, 5):
				sender_id = 5*n + i
				program("/dev/" + usbs[4 * n + i - 1], sender_id, 0)

	if rx:
		program('/dev/'+usbs[-1], 5*num_networks+1, 0)


def program_all_networks(num_networks, schedule_folder):

	# check if we have enough usbs
	(devs, stderr) = Popen(['ls','/dev/'], stdout=PIPE).communicate()

	usbs = re.findall("(ttyUSB.+)", devs.decode("utf-8"))
	print(usbs)

	if len(usbs) != num_networks*2+1:
		exit("USB number mismatch!")

	# assume two nodes per network
	schedule_files = [f for f in os.listdir(schedule_folder) if (isfile(join(schedule_folder, f)) and ("json" in f) and ("~" not in f))]

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

	program('/dev/'+usbs[-1], 2*num_networks+1, 0)



if __name__=='__main__':
	
	if len(sys.argv)!=5:
		exit('Usage: %s NUM_NETWORKS SCHEDULE_FOLDER IF_WHITE RX_TX' % sys.argv[0])

	num_networks = int(sys.argv[1])
	schedule_folder = str(sys.argv[2])
	is_white = int(sys.argv[3])
	is_rx = int(sys.argv[4])

	print(is_white)
	print(is_rx)

	if is_white:
		program_white_networks(num_networks, schedule_folder, is_rx)
	else:
		program_all_networks(num_networks, schedule_folder,is_rx)
	
