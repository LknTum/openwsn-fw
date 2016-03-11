#!/usr/bin/python
import sys, os, re
from subprocess import Popen, PIPE
import fileinput


gl_openwsn_dir = '../../'



def program(device, mote_id, is_dag):
	"""
	Program the mote with the predefined id
	"""
	assert(mote_id>0)

	# enable dag root
	for line in fileinput.input(gl_openwsn_dir + 'inc/opendefs.h', 	inplace=True):

		"""
		if '#define DAGROOT' in line:
			if is_dag:
				newline = '#define DAGROOT 1\n'
			else:
				newline = '//#define DAGROOT \n'
			line = line.replace(line, newline)
		"""
		sys.stdout.write(line)

	# change id
	for line in fileinput.input(gl_openwsn_dir + 'openstack/cross-layers/idmanager.c', inplace=True):

		if 'idmanager_vars.my64bID.addr_64b[7]   =' in line:
			newline = '   idmanager_vars.my64bID.addr_64b[7]   = %d;\n' % mote_id
			line = line.replace(line, newline)

		sys.stdout.write(line)

	os.system('./1b-BuildUploadHW.sh %s' % device)


if __name__=='__main__':

	if len(sys.argv)!=4:
		exit('Usage: %s USB_DEVICE MOTE_ID IS_DAG_ROOT' % sys.argv[0])

	device = sys.argv[1]
	mote_id = int(sys.argv[2])
	is_dag = int(sys.argv[3])

	program(device, mote_id, is_dag)
