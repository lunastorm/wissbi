#!/usr/bin/env python

import sys
import re
import socket

sep_re = re.compile("^==> (\/.*) <==$")

current_logger = ""
hostname = socket.gethostname()

while True:
    line = sys.stdin.readline()
    if len(line) == 0:
        break

    line = line.strip()
    if len(line) == 0:
        continue

    matcher = sep_re.match(line)
    if matcher != None:
        current_logger = matcher.group(1)
        continue

    sys.stdout.write("%s\t%s\t%s\n" % (hostname, current_logger, line))
    sys.stdout.flush()
