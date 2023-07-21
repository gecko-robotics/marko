#!/usr/bin/env python3

from pymarko.udpsocket import Publisher
from pymarko.udpsocket import Subscriber
import time
import sys

HOST, PORT = "10.0.1.116", 9999

def pub():
    pub = Publisher()
    pub.info()
    pub.clientaddr.append((HOST, PORT))
    pub.clientaddr.append((HOST, 9998))

    for _ in range(20):
        msg = str(time.time()).encode("utf-8")
        pub.publish(msg)

def sub():

    def cb(data):
        print(data)

    try:
        s = Subscriber()
        s.bind(HOST, PORT)
        s.info()
        s.register_cb(cb)
        s.loop()

    except KeyboardInterrupt as e:
        s.event = False
        time.sleep(0.1)
        print(e)
        print("ctrl-z")


if __name__ == "__main__":
    if sys.argv[1] == "p":
        pub()
    elif sys.argv[1] == "s":
        sub()
