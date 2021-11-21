#!/usr/bin/env python3
# -*- coding: utf-8 -*

import struct
import socket

IMAGE = 1
VECTORF = 4
VECTORD = 8

class UDPException(Exception):
    pass

class Data:
    MAX_PACKET_SIZE = 30000
    data = None
    manifest = None
    manifestFmt = "<LBB" # size, type, frames

    def pack(self):
        return self.manifest, self.data

    @staticmethod
    def readmanifest(msg):
        return struct.unpack("<LBB", msg)

class Vector(Data):
    def __init__(self, data, kind, size):
        numbytes = kind*size
        if numbytes > self.MAX_PACKET_SIZE:
            raise UDPException()
        fmt = 'f' if kind == 4 else 'd'
        self.data = [struct.pack(fmt*size, *data)]
        self.manifest = struct.pack('<LBB', numbytes, size, 1)

    @staticmethod
    def unpack(dbytes, kind, size):
        fmt = 'f' if kind == 4 else 'd'
        return struct.unpack(fmt*size, dbytes)


class Image(Data):
    def __init__(self, frame, jpeg_quality=80):
        encode_param = [int(cv2.IMWRITE_JPEG_QUALITY), jpeg_quality]
        ok, buffer = cv2.imencode('.jpg', frame, encode_param)
        buffer = buffer.tobytes()

        split = self.MAX_PACKET_SIZE
        num = dlen // self.MAX_PACKET_SIZE
        rem = dlen % self.MAX_PACKET_SIZE

        self.data = []
        for i in range(num):
            self.data.append(buffer[i*split:i*split+split])
        # self.data.append(buffer[-rem:])

        if rem > 0:
            self.data.append(buffer[-rem:])
            num += 1

        self.manifest = struct.pack('<LBB', self.size(), 2, num)


kind = VECTORD
v = Vector((-1.2,3.4,-5.6,-1.2,3.4,-5.6,), kind, 6)

manifest, buf = v.pack()
print(manifest)
print(Vector.readmanifest(manifest))
print(buf)
print(Vector.unpack(buf[0], kind, 6))



class SocketUDP:
    def __init__(self, maxpktsize=None):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        # self.MAX_PACKET_SIZE = maxpktsize or 30000

    def recvfrom(self):
        """
        Get data from remote host:
            1. get and read manifest file
            2. get data
        Return: bytes, address(ip, port)
        """
        manifest, address = self.sock.recvfrom(struct.calcsize(Data.manifestFmt))
        data_len,kind,num_packets = Data.readmanifest(manifest)

        # try:
        if num_packets == 1:
            data, server = self.sock.recvfrom(data_len)
        else:
            dlen = MAX_PACKET_SIZE #65000
            data = None
            for i in range(num_packets-1):
                d, server = self.sock.recvfrom(dlen)
                if data is None:
                    data = d
                else:
                    data += d

            dlen = data_len - (num_packets-1)*MAX_PACKET_SIZE #65000
            d, server = self.sock.recvfrom(dlen)
            data += d
        # except socket.timeout:
        #     pass

        if len(data) != data_len:
            print ("There was a image packet loss...")
            return None, None, address

        return data, kind, address

    def sendto(self, msg, address):
        manifest, data = msg.pack()
        self.sock.sendto(manifest, address)

        for d in data:
            self.sock.sendto(d, address)
