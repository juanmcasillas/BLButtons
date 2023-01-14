import socket
from time import sleep
import json

addr = ("127.0.0.1", 64998)
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
#sock.bind(addr)



cmd_mod =  json.dumps({ "cmd": "mod", "value": "UH-1H" })
cmd_exit =  json.dumps({ "cmd": "exit", "value": True   })

count = 0
while count < 5:
    print("simulating connection")
    sleep(1)
    count+=1

count = 0
while count < 100000:
    print(cmd_mod)
    sock.sendto(cmd_mod.encode("ascii"), addr)
    sleep(1)
    count+=1

sock.send(cmd_exit.encode("ascii"),addr)
sock.close()

