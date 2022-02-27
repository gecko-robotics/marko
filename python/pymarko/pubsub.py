# -*- coding: utf-8 -*-
##############################################
# The MIT License (MIT)
# Copyright (c) 2018 Kevin Walchko
# see LICENSE for full details
##############################################
from .udpsocket import SocketUDP
from .ip import get_ip
from threading import Thread
from colorama import Fore




class Base:
    def __init__(self):
        self.sock = SocketUDP(timeout=0.1)

    def bind(self, topic, port=None):
        addr = get_ip()
        self.sock.bind(addr,port)
        # addr, port = self.sock.bindaddress
        # print(f"{Fore.YELLOW}  Binding for {topic} on {addr}:{port} {Fore.RESET}")
        self.topic = topic

    def connect(self, topic, addr, port):
        self.sock.connect(addr,port)
        if topic is not None:
            self.sock.send(f"s:{topic}".encode("utf8"))
        self.topic = topic
        # print(f"{Fore.YELLOW}  Connect for {topic} on {addr}:{port} {Fore.RESET}")


class Publisher(Base):
    count = 0
    thread = None

    def __init__(self):
        super().__init__()
        # self.sock = SocketUDP()
        self.clientaddr = []

    def __del__(self):
        if self.thread:
            self.thread.join()

    def __listen(self):
        while True:
            data, addr = self.sock.recvfrom(100)
            if data:
                msg = data.decode('utf8')
                print(f">> Server got: {msg}")

                if msg == f's:{self.topic}':
                    self.clientaddr.append(addr)
                    print(f">> new {addr}")
                elif msg == "shutdown":
                    self.clientaddr.remove(addr)
                    print(f"xx shutdown {addr}")

    def listen(self):
        self.thread = Thread(target=self.__listen)
        self.thread.daemon = True
        self.thread.start()

    def publish(self, data):
        for addr in self.clientaddr:
            self.sock.sendto(data, addr)
            print(f"<< publish to: {addr}")


class Subscriber(Base):
    event = True
    cb = []
    def __init__(self):
        super().__init__()
        # self.sock = SocketUDP()

    def subscribe(self, callback, topic=None):
        # if topic is not None:
        #     self.sock.send(f"s:{topic}".encode("utf8"))
        self.cb.append(callback)

    def loop(self, event=None):
        # while self.event.isSet():
        while self.event:
            data = self.sock.recv(100)
            # if data is None or len(data) == 0:
            #     print("-- no data")
            #     continue
            for callback in self.cb:
                callback(data)
