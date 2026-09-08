import sys
import os
import _thread as thread
import threading
import socket
import atexit
import time
try:
    import msvcrt
    PLATFORM = "win"
except ImportError:
    PLATFORM = "unix"
    import tty
    import termios
    from select import select

def get_ch():
    if PLATFORM == "win":
        ch = msvcrt.getch()
        return ch
    elif PLATFORM == "unix":
        fd = sys.stdin.fileno()
        old_setting = termios.tcgetattr(fd)
        try:
            tty.setraw(sys.stdin.fileno())
            i, o, e = select([sys.stdin.fileno()], [], [], 5)
            if i:
                ch = sys.stdin.read(1)
            else:
                ch = ""
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, old_setting)
        return ch
    else:
        return ""


def udp_list(sock):
    print("start listening")
    packet_number = 0
    while 1:
        data, address = sock.recvfrom(1024)
        packet_number += 1
        print(f"time - {time.time()}")
        print(address,packet_number)
        print(data, len(data))
        data_s = []
        for i in range(0, len(data)):
            data_s.append(data[i])


def main():
    udp_port_self = 65500
    buffer_size = 1512
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    # Enable broadcasting mode
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    sock.bind(("", udp_port_self))
    print(f"UDP server ip - {socket.gethostbyname(socket.gethostname())}")
    thread.start_new_thread(udp_list, (sock,))
    message = b"UDP_REQUEST"
    while 1:
        q = get_ch()
        if len(q):
            print(q)
            if q == 'q':
                sock.close()
                time.sleep(0.3)
                sys.exit(1)
            elif ord(q) == 'u': #u
                print("send message",message)
                sock.sendto(message, ('192.168.0.255', 65500))


if __name__ == "__main__":
    main()
