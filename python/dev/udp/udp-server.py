#!/usr/bin/env python3

# import threading
import time
# from threading import Thread
# import socket
from pymarko import get_ip
import struct
# from collections import namedtuple
# MAX_PACKET_SIZE = 6000
# import numpy as np
from pymarko.udpsocket import Subscriber, Publisher

"""
sub.bind
sub.multicast -> pub.listen
pub.connect
pub.publish -> sub.subscribe
"""

if __name__ == "__main__":

    try:
        i = 0

        pub = Publisher()
        pub.bind("bob", 9500)
        # pub.connect("bob", get_ip(), 9500)
        pub.listen()
        i = 0
        while True:
            # d = struct.pack("i", i)
            pub.publish(b"hello")
            # print(i)
            i += 1
            time.sleep(1)

    except KeyboardInterrupt:
        print("shutting down")
    finally:
        print("end client ------------------")
