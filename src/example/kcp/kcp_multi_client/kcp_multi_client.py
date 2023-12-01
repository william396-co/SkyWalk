#!/usr/bin/python3
import socket
import threading
import time

threadcount = 1000
saddr = ('172.28.1.13', 9527)

def threadFunc():
    csd = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    while True:
        message = '[sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa]'
        csd.sendto(message.encode('utf-8'), saddr)
        time.sleep(0.1)
    csd.close()

if __name__ == "__main__":
    for i in range(threadcount):
        threading.Thread(target=threadFunc).start()
