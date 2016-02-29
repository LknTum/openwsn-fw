"""
This script is generating static schedule entries and a function for their initialization out of the configuration
provided in json format.
Default configuration file: schedule.json
Default output file: static_schedule.h
"""
__author__ = 'Mikhail Vilgelm'

import json
import datetime
import fileinput
import sys
from pprint import pprint


def read_config(fname):
    """
    Read configuration file as json object
    :param fname: file to read from
    :return: list of dicts
    """

    with open(fname) as data_file:
        data = json.load(data_file)

    pprint(data)

    return data


def generate_schedule(fname, path_to_schedule, path_to_schedule_params):
    """

    :param fname: input file in json format
    :param path_to_schedule:
    :param path_to_schedule_params:
    :return:
    """

    # read and parse config
    config = read_config(fname)

    active_slots = config["active_slots"]
    numserialrx = config["numserialrx"]
    numslotoff = config["numslotoff"]

    slotframe_length = len(active_slots) + numserialrx + numslotoff

    generate_schedule_parameters(path_to_schedule_params, slotframe_length, numserialrx, numslotoff)

    f = open(path_to_schedule, "w")

    f.write("/**\n\\brief Automatically generated from schedule.py\n\\%s\n*/\n" % str(datetime.datetime.now()))
    f.write("\n\n")
    f.write("#include \"schedule.h\"\n\n")
    f.write("void init_slotinfo();\n\n")
    f.write("slotinfo_element_t entries[%d];\n\n\n" % len(active_slots))
    f.write("void init_slotinfo(){\n\n")

    for idx, slot in enumerate(active_slots):
        f.write("\tentries[%d].slotOffset = %s;\n" % (idx, slot["slotOffset"]))
        f.write("\tentries[%d].link_type = %s;\n" % (idx, slot["link_type"]))
        f.write("\tentries[%d].shared = %s;\n" % (idx, slot["shared"]))
        f.write("\tentries[%d].channelOffset = %s;\n" % (idx, slot["channelOffset"]))

        for idx_addr, addr in enumerate(slot["address"].split(":")):
            f.write("\tentries[%d].address[%d] = %s;\n" % (idx, idx_addr, addr))

        f.write("\n\n")

    f.write("}\n")
    f.close()

def generate_schedule_parameters(path_to_schedule_params, slotframe_length, numserialrx, numslotoff):
    """
    Change schedule configuration parameters
    :param path_to_schedule_params: file to be changed
    :param slotframe_length:
    :param numserialrx:
    :param numslotoff:
    :return:
    """

    for line in fileinput.input(path_to_schedule_params, inplace=True):

        if '#define SLOTFRAME_LENGTH' in line:
            newline = '#define SLOTFRAME_LENGTH\t%d\n' % slotframe_length
            line = line.replace(line, newline)

        elif '#define NUMSERIALRX' in line:
            newline = '#define NUMSERIALRX\t%d\t///< @lkn{Samu} Number of slots used for serial transmission\n' \
                      % numserialrx
            line = line.replace(line, newline)

        elif '#define NUMSLOTSOFF' in line:
            newline = '#define NUMSLOTSOFF\t%d\t///< @lkn{Samu} Number of slots NOT used, \
            they are probably used in for serial transmission anyway.\n' % numslotoff
            line = line.replace(line, newline)

        sys.stdout.write(line)

    fileinput.close()



if __name__ == "__main__":

    # parse command-line arguments, if any
    if len(sys.argv) == 1:
        # use default values
        config_file = "schedule.json"
        path_to_schedule = "../../openstack/02b-MAChigh/static_schedule.h"
        path_to_schedule_params = "../../openstack/02b-MAChigh/schedule.h"
    else:
        if len(sys.argv) != 4:
            exit("Usage: %s [config_file output_file_static output_file_schedule_params]" % sys.argv[0])
        config_file = sys.argv[1]
        path_to_schedule = sys.argv[2]
        path_to_schedule_params = sys.argv[3]

    # do the work
    generate_schedule(config_file, path_to_schedule, path_to_schedule_params)
