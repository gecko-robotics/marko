#!/usr/bin/env python3

import sys
import time
from pymarko.udpsocket import Request, Reply


HOST, PORT = "10.0.1.116", 9999


def rep():
    def cb(data):
        print(data)
        return data

    try:
        r = Reply((HOST,PORT))
        r.register_cb(cb)
        r.loop()
    except KeyboardInterrupt:
        print("cntrl-z")


def req():
    msg = f"{time.time()}".encode("utf-8")
    r = Request()
    ans = r.request(msg,(HOST,PORT))
    print(f"got: {ans}")


if __name__ == "__main__":
    if sys.argv[1] == "rq":
        req()
    elif sys.argv[1] == "rp":
        rep()
