import sys
import os
import _thread as thread
import threading
import socket
import atexit
import time
import msvcrt


def udp_list(sock):
    print("start listening")
    packet_number = 0    
    while 1:
        data, address = sock.recvfrom(1024)
        packet_number += 1
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
    thread.start_new_thread(udp_list, (sock,))
    message = b"UDP_REQUEST"
    while 1:
        q = msvcrt.getch()
        print(q)
        if ord(q) == 113: #q
            s.close()
            sys.exit(1)
        elif ord(q) == 117: #u
            print("send message",message)
            sock.sendto(message, ('192.168.0.255', 65500))


if __name__ == "__main__":
    main()
