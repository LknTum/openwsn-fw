"""
This script is generating static schedule entries and a function for their initialization out of the configuration
provided in json format.
Default configuration file: schedule.json
Default output file: static_schedule.h
"""
__author__ = 'Mikhail Vilgelm'

import json
import datetime
from pprint import pprint


config_file = "schedule.json"
path_to_schedule = "../../openstack/02b-MAChigh/static_schedule.h"


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


def generate_schedule(fname, path_to_schedule):

    f = open(path_to_schedule, "w")

    f.write("/**\n\\brief Automatically generated from schedule.py\n\\%s\n*/\n" % str(datetime.datetime.now()))
    f.write("\n\n")
    f.write("#include \"schedule.h\"\n\n")

    f.write("void init_slotinfo();\n\n")

    entries = read_config(fname)

    f.write("slotinfo_element_t entries[%d];\n\n\n" % len(entries))

    f.write("void init_slotinfo(){\n\n")

    for idx, entry in enumerate(entries):
        f.write("entries[%d].slotOffset = %s;\n" % (idx, entry["slotOffset"]))
        f.write("entries[%d].link_type = %s;\n" % (idx, entry["link_type"]))
        f.write("entries[%d].shared = %s;\n" % (idx, entry["shared"]))
        f.write("entries[%d].channelOffset = %s;\n" % (idx, entry["channelOffset"]))

        for idx_addr, addr in enumerate(entry["address"].split(":")):
            f.write("entries[%d].address[%d] = %s;\n" % (idx, idx_addr, addr))

        f.write("\n\n")

    f.write("}\n")
    f.close()


if __name__ == "__main__":

    generate_schedule(config_file, path_to_schedule)
