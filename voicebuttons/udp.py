import socket
from time import sleep
import json
from types import SimpleNamespace

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
sock.bind(("127.0.0.1", 64998))



while True:
    data, addr = sock.recvfrom(1024)
    try:
        msg = json.loads(data, object_hook=lambda d: SimpleNamespace(**d))
    except Exception as e:
        print("error parsing data: %s" % e)
        continue

    # 
    # process here the cmd
    # 
    if msg.cmd == "mod":
        print("loading mod %s" % msg.value)
        continue

    if msg.cmd == "exit" and msg.value:
        print("simulation exit")
        continue


    sleep(0.1)