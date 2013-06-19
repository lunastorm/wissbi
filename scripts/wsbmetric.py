#!/usr/bin/env python

import os
import rrdtool
from ConfigParser import ConfigParser
from gmetric import Gmetric

conf = ConfigParser()
conf.read("/etc/wissbi.conf")
rrd_folder = conf.get("metricd", "rrd_folder")

counter_map = {}

def rrd_name(name, direction):
    return "wissbi.%s.%s" % (name.replace("/", "__"), direction)

def load_counter(name):
    try:
        counter_map[name] = int(rrdtool.info("%s/%s.rrd" % (rrd_folder, name))["ds[sum].last_ds"])
    except:
        counter_map[name] = 0

metric_sender = Gmetric("239.2.11.71", 8649, "multicast")
while True:
    line = os.sys.stdin.readline().strip()
    if len(line) == 0:
        break

    (name, direction, value) = line.split(",")
    name = rrd_name(name, direction)
    if not name in counter_map:
        load_counter(name)

    new_counter = counter_map[name] + int(value)
    if new_counter > 0xffffffff:
        new_counter = new_counter - 0xffffffff - 1

    metric_sender.send(name, new_counter, TYPE="uint32", SLOPE="positive", GROUP="wissbi")
    counter_map[name] = new_counter

