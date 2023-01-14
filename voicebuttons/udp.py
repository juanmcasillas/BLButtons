import socket
from time import sleep

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
sock.bind(("127.0.0.1", 64998))
while True:
    data, addr = sock.recvfrom(1024)
    print(data)
    sleep(0.1)