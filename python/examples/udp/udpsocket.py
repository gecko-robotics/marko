import socket
from pymarko import get_ip

MAX_PACKET_SIZE = 6000

"""
sub.bind
sub.multicast -> pub.listen
pub.connect
pub.publish -> sub.subscribe
"""


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

    def subscribe(self, callback, topic=None):
        if topic is not None:
            self.sock.send(f"s:{topic}".encode("utf8"))
        self.cb.append(callback)

    def loop(self, event=None):
        # while self.event.isSet():
        while self.event:
            data = self.sock.recv(100)
            if data is None or len(data) == 0:
                print("-- no data")
                continue
            for callback in self.cb:
                callback(data)
