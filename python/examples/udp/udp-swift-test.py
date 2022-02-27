#!/usr/bin/env python3

# import threading
import time
from threading import Thread
# import socket
from pymarko import get_ip
import struct
# from collections import namedtuple
MAX_PACKET_SIZE = 6000
# import numpy as np

"""
sub.bind
sub.multicast -> pub.listen
pub.connect
pub.publish -> sub.subscribe
"""


from udpsocket import Subscriber, Publisher

if __name__ == "__main__":

    try:
        # while True:
        # def sub(data):
        #     # d = unpacker.unpack(data)
        #     print(f">> Received {data}[{len(data)}]")
        i = 0
        def myfunc(data):
            global i
            # d = unpacker.unpack(data)
            # print(f">> Received {d}[{len(data)}]")
            if data:
                msg = data.decode('utf8')
                print(f">> Subscriber got {i}: {msg}")
                i += 1

        sub = Subscriber()
        # sub.bind("bob", 9500)
        sub.connect("bob", get_ip(), 9500)
        # sub.connect("bob", "127.0.0.1", 9500)
        # sub.connect("bob", "10.0.1.199", 9500)
        sub.subscribe(myfunc)
        sub.loop()

    except KeyboardInterrupt:
        print("shutting down")
    finally:
        print("end client ------------------")
