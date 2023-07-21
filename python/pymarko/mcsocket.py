# -*- coding: utf-8 -*-
##############################################
# The MIT License (MIT)
# Copyright (c) 2018 Kevin Walchko
# see LICENSE for full details
##############################################
import time
import socket


class MCSocket:
    pass


class MCBeacon:
    event = True
    socket = None
    group = None

    def __init__(self, group):
        self.group = group
        self.socket = MCSocket()

    def __listen(self, topic, address):
        while True:
            data, addr = self.socket.recvfrom(100)
            if data is not None and len(data) > 0:
                try:
                    msg = data.decode('utf8')
                    print(f">> Server got {msg} from {addr}")

                    # msg - s:topic
                    # return - ip:port
                    if msg == f"s:{topic}":
                        self.socket.sendto(f"{address[0]}:{address[1]}", addr)
                except:
                    continue

    def listen(self, topic, addr):
        self.thread = Thread(target=self.__listen, args=(topic, addr,))
        self.thread.daemon = True
        self.thread.start()

    def search(self, data, datasize=100, delay=0.01, cnt=5):
        """Find server beacon"""
        self.socket.sendto(data, self.group)

        while cnt:
            data, addr = self.socket.recvfrom(datasize)
            if data is None or len(data) == 0:
                cnt -= 1
                time.sleep(delay)
            else:
                break
        return data

    def broadcast(self, msg, delay=1.0):
        """Periodically send message out"""
        while self.event:
            time.sleep(delay)
            self.socket.sendto(msg, self.group)
