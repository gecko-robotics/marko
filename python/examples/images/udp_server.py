#!/usr/bin/env python3
# -*- coding: utf-8 -*
##############################################
# The MIT License (MIT)
# Copyright (c) 2014 Kevin Walchko
# see LICENSE for full details
##############################################
# for macOS
# https://stackoverflow.com/questions/22819214/udp-message-too-long
# sudo sysctl -w net.inet.udp.maxdgram=65535
#
import time
import socket
import cv2
from threading import Thread, Lock
import struct
import argparse
from colorama import Fore
# from opencv_camera import __version__ as version
# from snu.utils import bgr2gray
# from snu.utils import MAX_PACKET_SIZE
from pymarko import get_ip
from pymarko.utils import MAX_PACKET_SIZE
# from slurm.network import get_ip

bgr2gray = lambda x: cv2.cvtColor(x, cv2.COLOR_BGR2GRAY)

debug = True
host_name = socket.gethostname()

class VideoGrabber(Thread):
        def __init__(self, jpeg_quality, size=480, source=0, gray=False):
            Thread.__init__(self)
            self.encode_param = [int(cv2.IMWRITE_JPEG_QUALITY), jpeg_quality]
            self.cap = cv2.VideoCapture(source)
            self.grayscale = gray
            # 1280x720
            # 640x480
            # 320x240
            if size == 120:
                self.cap.set(3, 160)
                self.cap.set(4, 120)
            elif size == 240:
                self.cap.set(3, 320)
                self.cap.set(4, 240)
            elif size == 480:
                self.cap.set(3, 640)
                self.cap.set(4, 480)
            elif size == 720:
                self.cap.set(3, 1280)
                self.cap.set(4, 720)
            else:
                print(f"{Fore.RED}*** Invalide image size: {size} ***{Fore.RESET}")
                print("Using camera default")

            self.running = True
            self.buffer = None
            self.lock = Lock()

        def stop(self):
            self.running = False

        def get_buffer(self):

            if self.buffer is not None:
                    self.lock.acquire()
                    # cpy = self.buffer.copy()
                    cpy = self.buffer.tobytes()
                    self.lock.release()
                    return cpy

        def run(self):
            while self.running:
                ok, img = self.cap.read()
                if not ok:
                    continue

                if self.grayscale:
                    # img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
                    img = bgr2gray(img)

                # print(img.shape)
                self.lock.acquire()
                ok, buffer = cv2.imencode('.jpg', img, self.encode_param)
                # print(f"img: {img.shape} {img.size}")
                # print(f"jpg: {len(buffer)}")
                # print()
                if ok:
                    self.buffer = buffer
                self.lock.release()


# class UDPStreamer:
#     pass
#
# class UDP:
#     pass


def handle_args():
    # parser = argparse.ArgumentParser(version=VERSION, description='A simple \
    parser = argparse.ArgumentParser(description=f'A simple \
    program to capture images from a camera and send them over the network \
    as a UDP message. Unfortunately, you cannot send large images. The \
    messages are limited to 65507 bytes. So the larger the image, the lower \
    jpeg quality needs to be. You can easily do 240 @ 100% or 480 @ 95% \
    or 720 @ 65%.')

    parser.add_argument('-c', '--camera', help='which camera to use, default is 0', default=0)
    parser.add_argument('-g', '--grayscale', action='store_true', help='capture grayscale images, reduces data size', default=False)
    parser.add_argument('--host', help='host ip address', default=None)
    parser.add_argument('-q', '--quality', type=int, help='jpeg quality percentage, default is 80', default=80)
    parser.add_argument('-p','--port', help='port, default is 9050', default=9050)
    parser.add_argument('-s', '--size', type=int, help='size of image capture (480=(640x480), 720=(1280x720)), default 240', default=240)
    # parser.add_argument('-v', '--version', action='store_true', help='returns version number')

    return vars(parser.parse_args())

# Manifest = namedtupe("Manifest", "size type dataframes")

class Manifest:
    def __init__(self, size, type, dataframes):
        self.size = size
        self.type = type
        self.df = dataframes

    def tobytes(self):
        return struct.pack('<LBB', self.size, self.type, self.df)

class Data:
    type = None

    def manifest(self):
        return struct.pack('<LBB', self.size(), self.type, self.dataframe())
    def size(self):
        return 0
    def dataframe(self):
        return 0
    def buffer(self):
        pass

class Vector(Data):
    __data = None

    def __init__(self, x=0,y=0,z=0):
        self.__data = [x,y,z]
        self.type = 1

# class Image(Data):
#     def __init__()




class SocketUDP:
    def __init__(self, maxpktsize=MAX_PACKET_SIZE):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.MAX_PACKET_SIZE = MAX_PACKET_SIZE # maxpktsize or 30000

    def recvfrom(self, size):
        """
        Get data from remote host
        Return: data, address
        """
        data, address = self.sock.recvfrom(struct.calcsize('<L'))
        data = struct.unpack('<L', data)
        return data, address

    def sendto(self, data, address):
        dlen = data.size
        if dlen > self.MAX_PACKET_SIZE:
            split = self.MAX_PACKET_SIZE
            num = dlen // split
            rem = dlen % split
            # print(f"{num} {rem}")
            sock.sendto(struct.pack('<LB',dlen, num+1), address)

            for i in range(num):
                sock.sendto(data[i*split:i*split+split], address)
            sock.sendto(buffer[-rem:], address)
        else:
            sock.sendto(struct.pack('<LB', dlen, 1), address)
            sock.sendto(data, address)
        return dlen

def main():
    args = handle_args()

    # if args["version"]:
    #     print(f">> udp_server version {version}")
    #     exit(0)

    port = args["port"]
    host = args["host"]
    if host is None:
        host = get_ip()
    jpeg_quality = args["quality"]
    size = args["size"]
    camera = args["camera"]
    gray = args["grayscale"]
    # gray = True

    grabber = VideoGrabber(jpeg_quality, size, camera, gray)
    grabber.daemon = True
    grabber.start()

    running = True

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # Bind the socket to the port
    server_address = (host, port)
    # address = server_address

    print(f'{host_name}[{host}]:{port} at {size}\n')

    sock.bind(server_address)
    try:
        while running:
            try:
                data_packed, address = sock.recvfrom(struct.calcsize('<L'))
                data = struct.unpack('<L',data_packed)[0]
                if data == 1:
                    buffer = grabber.get_buffer()

                    if buffer is None:
                        sock.sendto(struct.pack('<L',struct.calcsize('<L')), address)
                        sock.sendto(struct.pack('<L',404), address) #capture error
                        continue

                    # print(len(buffer))

                    if len(buffer) > MAX_PACKET_SIZE:
                        split = MAX_PACKET_SIZE #65000
                        num = len(buffer)//split
                        rem = len(buffer)%split
                        print(f"{num} {rem}")
                        sock.sendto(struct.pack('<LB',len(buffer), num+1), address)

                        for i in range(num):
                            sock.sendto(buffer[i*split:i*split+split], address)
                        sock.sendto(buffer[-rem:], address)
                        continue
                    sock.sendto(struct.pack('<LB',len(buffer), 1), address)
                    sock.sendto(buffer, address)
                elif data == 0:
                    grabber.stop()
                    running = False
            except Exception as e:
                bb = buffer
                # print(bb)
                print(f"{Fore.RED}*** {e} buffer: {len(bb)} {type(bb)} ***{Fore.RESET}")
                time.sleep(1)
    except KeyboardInterrupt:
        print("ctrl-C ...")

    grabber.stop()
    running = False
    print("Quitting..")
    grabber.join()
    sock.close()


if __name__ == '__main__':
    main()
