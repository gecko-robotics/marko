#!/usr/bin/env python
##############################################
# The MIT License (MIT)
# Copyright (c) 2018 Kevin Walchko
# see LICENSE for full details
##############################################
#
# https://pymotw.com/2/socket/multicast.html
#
import socket
import select
import struct
import time
import sys
import traceback
import struct

from pymarko.mcsocket import MultiCastSocket


if len(sys.argv) != 2:
    print("Usage: ./mc.py message")
    exit(1)

msg = sys.argv[1].encode("utf8")
mc = MultiCastSocket(group=('224.0.0.251', 5353), ttl=2, timeout=1)
print(">>", mc.info())

while True:
    try:
        # time.sleep(0.5)

        # mc.cast(msg)
        # time.sleep(3)
        # print(">> beacon sent: {}".format(msg))
        data, address = mc.recv()
        # data, address = mc.recv_nb()
        if data is None:
            continue

        print(">> {} from {}".format(data, address))

    except KeyboardInterrupt:
        print("ctrl-z")
        # traceback.print_exc()
        break
