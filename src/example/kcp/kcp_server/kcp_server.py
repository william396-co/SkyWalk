#!/usr/bin/python3
import select
import socket

sd = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sd.bind(('0.0.0.0', 9527))
sd.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, True)
sd.setblocking(False)
fd2socket = {sd.fileno():sd, }

READ_ONLY = (select.EPOLLIN | select.EPOLLPRI | select.EPOLLHUP | select.EPOLLERR)
epoller = select.epoll()
epoller.register(sd, READ_ONLY)

if __name__ == '__main__':
    while True:
        events = epoller.poll()
        for fd, flag in events:
            s = fd2socket[fd]
            if s == sd:
                data, addr = s.recvfrom(1024)
                #
                csd = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                csd.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, True)
                csd.bind(('0.0.0.0', 9527))
                csd.connect(addr)
                fd2socket[csd.fileno()] = csd
                epoller.register(csd, READ_ONLY)
                print('accept connection from %s:%s' % addr, 'sd[%d] created' % csd.fileno())
            else:
                data, addr = s.recvfrom(1024)
                if data:
                    print('received "%s" from %s' % (data.splitlines()[0].decode('utf-8', 'ignore'), addr), 'sd[%d]' % s.fileno())
                    s.sendto(data, addr)
    sd.close()
