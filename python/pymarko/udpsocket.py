# -*- coding: utf-8 -*-
##############################################
# The MIT License (MIT)
# Copyright (c) 2018 Kevin Walchko
# see LICENSE for full details
##############################################
import socket
import time

MAX_PACKET_SIZE = 6000


class UDPSocket:
    sock = None

    """
    UDP doesn't have to connect to send/receive data to a server.
    """
    def __init__(self, maxpktsize=MAX_PACKET_SIZE, timeout=None):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        if timeout is not None:
            self.sock.settimeout(timeout)
        self.MAX_PACKET_SIZE = MAX_PACKET_SIZE # maxpktsize or 30000

    def __del__(self):
        self.sock.close()

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
        except ConnectionRefusedError:
            a,p = self.sock.getpeername()
            raise ConnectionRefusedError(f"*** ConnectionRefusedError {a}:{p} ***")

        return data, address

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


class Base:
    socket = None
    bind_addr = None

    def __init__(self):
        self.socket = UDPSocket()

    def info(self):
        print(f"[ SocketUDP ]============================")
        print(f"  proto: {self.socket.sock.proto}")
        print(f"  timeout: {self.socket.sock.timeout}")
        print(f"  family: {self.socket.sock.family}")
        print(f"  timeout: {self.socket.sock.type}")
        print(f"  blocking: {self.socket.sock.getblocking()}")
        print(f"  fileno: {self.socket.sock.fileno()}")
        # print("  remote:",self.socket.sock.getpeername())
        print("  local:",self.socket.sock.getsockname())
        print(f"")

    def bind(self, address, port=None):
        port = 0 if port is None else port
        self.socket.sock.bind((address, port))
        self.bind_addr = self.socket.sock.getsockname()
        # print(f">> Binding for on {addr}:{port}")


class Subscriber(Base):
    event = True
    cb = [] # why array?
    subscribedto = None

    def __init__(self):
        super().__init__()
        # self.socket.sock.settimeout(1.0)

    def __del__(self):
        if self.subscribedto is not None:
            self.socket.sendto(f"shutdown".encode("utf8"), self.subscribedto)

    def register_cb(self, callback):
        self.cb.append(callback)

    def loop(self, datasize=100):
        while self.event:
            data, addr = self.socket.recvfrom(datasize)
            if data is None or len(data) == 0:
                # print("-- no data")
                continue

            for callback in self.cb:
                callback(data)

class Publisher(Base):
    clientaddr = []

    def __init__(self):
        super().__init__()

    def publish(self, data):
        for addr in self.clientaddr:
            self.socket.sendto(data, addr)


class PublisherThread(Publisher):
    thread = None

    def __init__(self):
        super().__init__()

    def __del__(self):
        if self.thread is not None:
            self.thread.join()

    def __listen(self):
        while True:
            data, addr = self.socket.recvfrom(100)
            if data:
                # print(f">> Server got: {data}")
                try:
                    msg = data.decode('utf8')
                except:
                    continue

                if msg == "subscribe":
                    self.clientaddr.append(addr)
                    # print(f">> new {addr}")
                elif msg == "shutdown":
                    try:
                        self.clientaddr.remove(addr)
                        print(f"xxx shutdown {addr} xxx")
                    except:
                        pass

    def listen(self):
        self.thread = Thread(target=self.__listen)
        self.thread.daemon = True
        self.thread.start()



##########################################################

class Reply(Base):
    event = True
    cb = [] # why array?

    def __init__(self, addr):
        super().__init__()
        a,p = addr
        self.bind(a,p)

    def register_cb(self, callback):
        self.cb.append(callback)

    def loop(self, datasize=100):
        while self.event:
            data, addr = self.socket.recvfrom(datasize)
            if data is None or len(data) == 0:
                # print("-- no data")
                continue
            # self.subscribedto = addr
            # print(addr)
            for callback in self.cb:
                reply = callback(data)
                if reply is not None:
                    self.socket.sendto(reply, addr)

class Request(Base):
    def __init__(self):
        super().__init__()

    def request(self, data, addr, datasize=100):
        self.socket.sendto(data, addr)
        data, _ = self.socket.recvfrom(datasize)
        return data
