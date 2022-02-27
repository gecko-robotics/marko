#!/usr/bin/env python3

# import threading
import time
from threading import Thread
# import socket
from pymarko import get_ip
# import struct
# from collections import namedtuple
# MAX_PACKET_SIZE = 6000
# import numpy as np
import sys
from pymarko.pubsub import Subscriber, Publisher

"""
sub.bind
sub.multicast -> pub.listen
pub.connect
pub.publish -> sub.subscribe
"""

def subfunc():

    def myfunc(data):
        if data:
            msg = data.decode('utf8')
            print(f">> Subscriber got: {msg}")

    sub = Subscriber()
    # sub.bind("bob", 9500)
    sub.connect("bob", get_ip(), 9500)
    # sub.connect("bob", "127.0.0.1", 9500)
    # sub.connect("bob", "10.0.1.199", 9500)
    sub.subscribe(myfunc)
    sub.loop()

def pubfunc():
    pub = Publisher()
    pub.bind("bob", 9500)
    pub.listen()

    i = 0
    while True:
        msg = f"hello {i}".encode("utf8")
        pub.publish(msg)
        time.sleep(1)
        i += 1


if __name__ == "__main__":

    if len(sys.argv) > 1:
        func = sys.argv[1]
    else:
        func = "p"

    try:
        if func == "s":
            subfunc()
        elif func == "p":
            pubfunc()

    except KeyboardInterrupt:
        print("shutting down")
    finally:
        print("end ------------------")
