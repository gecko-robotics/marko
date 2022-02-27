#!/usr/bin/env python3

import threading
import time
import socket
from pymarko import get_ip
import struct
from collections import namedtuple
MAX_PACKET_SIZE = 6000
# import numpy as np

"""
sub.bind
sub.multicast -> pub.listen
pub.connect
pub.publish -> sub.subscribe
"""


geckoIP = namedtuple("geckoIP", "address port")

class Sensor:
    id = 0
    def __init__(self, fmt):
        self.fmt = fmt

    def pack(self, data):
        self.id += 1
        return struct.pack(self.fmt, data[0], data[1], data[2], self.id)

    def unpack(self, data):
        return struct.unpack(self.fmt, data)




class SocketUDP:
    def __init__(self, maxpktsize=MAX_PACKET_SIZE, timeout=None):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        if timeout is not None:
            self.sock.settimeout(timeout)
        self.MAX_PACKET_SIZE = MAX_PACKET_SIZE # maxpktsize or 30000

        self.connect = self.sock.connect
        # self.send = self.sock.send
        print(f"sudp proto: {self.sock.proto}")
        print(f"sudp timeout: {self.sock.timeout}")
        print(f"sudp family: {self.sock.family}")
        print(f"sudp timeout: {self.sock.type}")
        print(f"sudp blocking: {self.sock.getblocking()}")
        print(f"sudp fileno: {self.sock.fileno()}")

    def recv(self, size):
        """
        Get data from remote host
        Return: data
        """
        try:
            data = self.sock.recv(size) #struct.calcsize('<L'))
        except socket.timeout:
            data = None
        # data = struct.unpack('<L', data)
        return data

    def recvfrom(self, size):
        """
        Get data from remote host
        Return: data, address
        """
        data, address = self.sock.recvfrom(size)
        return data, address

    def send(self, data):
        dlen = len(data)

        if dlen > self.MAX_PACKET_SIZE:
            split = self.MAX_PACKET_SIZE
            num = dlen // split
            rem = dlen % split
            # print(f"{num} {rem}")
            # self.sock.sendto(struct.pack('<LB',dlen, num+1), address)

            for i in range(num):
                self.sock.send(data[i*split:i*split+split])
            self.sock.send(buffer[-rem:])
        else:
            # self.sock.sendto(struct.pack('<LB', dlen, 1), address)
            self.sock.send(data)
        return dlen

    def sendto(self, data, address):
        dlen = len(data)

        if dlen > self.MAX_PACKET_SIZE:
            split = self.MAX_PACKET_SIZE
            num = dlen // split
            rem = dlen % split
            # print(f"{num} {rem}")
            # self.sock.sendto(struct.pack('<LB',dlen, num+1), address)

            for i in range(num):
                self.sock.sendto(data[i*split:i*split+split], address)
            self.sock.sendto(buffer[-rem:], address)
        else:
            # self.sock.sendto(struct.pack('<LB', dlen, 1), address)
            self.sock.sendto(data, address)
        return dlen

    def bind(self, address, port=None):
        port = 0 if port is None else port
        server_address = (address, port)
        self.sock.bind(server_address)
        self.bindaddress = self.sock.getsockname()


class Base:
    def bind(self, topic, port=None):
        addr = get_ip()
        self.sock.bind(addr,port)
        addr, port = self.sock.bindaddress
        print(f">> Binding for {topic} on {addr}:{port}")
        self.topic = topic

    def connect(self, topic, addr, port):
        addr = get_ip()
        port = 9500
        self.sock.connect((addr,port))
        print("Client:")
        print("  remote:",self.sock.sock.getpeername())
        print("  local:",self.sock.sock.getsockname())
        self.topic = topic


class Publisher(Base):
    count = 0

    def __init__(self):
        self.sock = SocketUDP()
        self.clientaddr = []

    def listen(self):
        while True:
            data, addr = self.sock.recvfrom(100)
            msg = data.decode('utf8')
            print(f">> Server got: {msg}")

            if msg == f's:{self.topic}':
                self.clientaddr.append(addr)
                break

    def publish(self, data):
        for addr in self.clientaddr:
            self.sock.sendto(data, addr)
            print(f"<< publish to: {addr}")


class Subscriber(Base):
    event = True
    cb = []
    def __init__(self):
        self.sock = SocketUDP()

    def subscribe(self, topic, callback):
        self.sock.send(f"s:{topic}".encode("utf8"))
        self.cb.append(callback)

    def loop(self):
        while self.event.isSet():
            data = self.sock.recv(100)
            if data is None or len(data) == 0:
                print("-- no data")
                continue
            for callback in self.cb:
                callback(data)



services = {}

def server(e):
    pub = Publisher()
    pub.bind("bob", 9500)
    pub.listen()

    packer = Sensor("<3dL")
    i = 0
    while e.isSet():
        print(f"<< {i}")
        p = packer.pack((i,i,i,))
        pub.publish(p)
        time.sleep(0.1)
        i += 1

def client(e):
    unpacker = Sensor("<3dL")
    def myfunc(data):
        d = unpacker.unpack(data)
        print(f">> Received {d}[{len(data)}]")

    sub = Subscriber()
    sub.connect("bob", get_ip(), 9500)
    sub.event = e

    sub.subscribe("bob", myfunc)
    sub.loop()
    print("end client ------------------")


if __name__ == "__main__":
    e = threading.Event()
    e.set()

    s = threading.Thread(target=server, args=(e,))
    s.daemon = True
    s.start()

    c = threading.Thread(target=client, args=(e,))
    c.daemon = True
    c.start()

    try:
        while True:
            time.sleep(0.5)
    except KeyboardInterrupt:
        print("shutting down")
    finally:
        e.clear()
        s.join(timeout=1)
        c.join(timeout=1)
