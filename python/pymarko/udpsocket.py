# -*- coding: utf-8 -*-
##############################################
# The MIT License (MIT)
# Copyright (c) 2018 Kevin Walchko
# see LICENSE for full details
##############################################
import socket
from pymarko.ip import get_ip
# from threading import Thread
from colorama import Fore

MAX_PACKET_SIZE = 6000

"""
sub.bind
sub.multicast -> pub.listen
pub.connect
pub.publish -> sub.subscribe
"""


class SocketUDP:
    """
    UDP doesn't have to connect to send/receive data to a server.
    """
    def __init__(self, maxpktsize=MAX_PACKET_SIZE, timeout=None):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        if timeout is not None:
            self.sock.settimeout(timeout)
        self.MAX_PACKET_SIZE = MAX_PACKET_SIZE # maxpktsize or 30000

        # self.connect = self.sock.connect
        # self.send = self.sock.send
        print(f"{Fore.GREEN}[ SocketUDP ]============================")
        print(f"{Fore.CYAN}  proto: {self.sock.proto}")
        print(f"  timeout: {self.sock.timeout}")
        print(f"  family: {self.sock.family}")
        print(f"  timeout: {self.sock.type}")
        print(f"  blocking: {self.sock.getblocking()}")
        print(f"  fileno: {self.sock.fileno()}")
        print(f"{Fore.RESET}")

    def __del__(self):
        self.sock.close()

    def recv(self, size):
        """
        Get data from remote host
        Return: data
        """
        try:
            data = self.sock.recv(size) #struct.calcsize('<L'))
        except socket.timeout:
            data = None
        except ConnectionRefusedError:
            a,p = self.sock.getpeername()
            print(f"{Fore.RED}*** ConnectionRefusedError {a}:{p} ***{Fore.RESET}")
            exit(1)
        # data = struct.unpack('<L', data)
        return data

    def recvfrom(self, size):
        """
        Get data from remote host
        Return: data, address
        """
        try:
            data, address = self.sock.recvfrom(size)
        except socket.timeout:
            data = None
            address = None
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

    def connect(self, address, port):
        """
        Connect sets the socket to (addr, port) and must use send/recv calls
        to get/give data with.
        """
        self.sock.connect((address, port))
        print("Connect:")
        print("  remote:",self.sock.getpeername())
        print("  local:",self.sock.getsockname())

    def bind(self, address, port=None):
        """
        Bind doesn't limit the socket to one host/port and must use sendto/recvfrom
        to get/give data with.
        """
        port = 0 if port is None else port
        server_address = (address, port)
        self.sock.bind(server_address)
        # self.bindaddress = self.sock.getsockname()
        addr, port = self.sock.getsockname()
        print(f">> Binding for on {addr}:{port}")

#
# class Base:
#     def __init__(self):
#         self.sock = SocketUDP(timeout=0.1)
#         print(">> Base init")
#
#     def bind(self, topic, port=None):
#         addr = get_ip()
#         self.sock.bind(addr,port)
#         # addr, port = self.sock.bindaddress
#         # print(f">> Binding for {topic} on {addr}:{port}")
#         self.topic = topic
#
#     def connect(self, topic, addr, port):
#         self.sock.connect(addr,port)
#         if topic is not None:
#             self.sock.send(f"s:{topic}".encode("utf8"))
#         self.topic = topic
#
#
# class Publisher(Base):
#     count = 0
#     thread = None
#
#     def __init__(self):
#         super().__init__()
#         # self.sock = SocketUDP()
#         self.clientaddr = []
#
#     def __del__(self):
#         if self.thread:
#             self.thread.join()
#
#     def __listen(self):
#         while True:
#             data, addr = self.sock.recvfrom(100)
#             if data:
#                 msg = data.decode('utf8')
#                 print(f">> Server got: {msg}")
#
#                 if msg == f's:{self.topic}':
#                     self.clientaddr.append(addr)
#                     print(f">> new {addr}")
#                 elif msg == "shutdown":
#                     self.clientaddr.remove(addr)
#                     print(f"xx shutdown {addr}")
#
#     def listen(self):
#         self.thread = Thread(target=self.__listen)
#         self.thread.daemon = True
#         self.thread.start()
#
#     def publish(self, data):
#         for addr in self.clientaddr:
#             self.sock.sendto(data, addr)
#             print(f"<< publish to: {addr}")
#
#
# class Subscriber(Base):
#     event = True
#     cb = []
#     def __init__(self):
#         super().__init__()
#         # self.sock = SocketUDP()
#
#     def subscribe(self, callback, topic=None):
#         # if topic is not None:
#         #     self.sock.send(f"s:{topic}".encode("utf8"))
#         self.cb.append(callback)
#
#     def loop(self, event=None):
#         # while self.event.isSet():
#         while self.event:
#             data = self.sock.recv(100)
#             # if data is None or len(data) == 0:
#             #     print("-- no data")
#             #     continue
#             for callback in self.cb:
#                 callback(data)
