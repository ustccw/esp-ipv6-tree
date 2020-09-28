#!/usr/bin/env python
#-*- coding: utf-8 -*-
# chenwu@espressif.com

import socket

UDP_IP = "::"
UDP_PORT = 59652    # port of magic number

udp6_socket = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
udp6_socket.bind((UDP_IP, UDP_PORT))

while True:
    data, addr = udp6_socket.recvfrom(1460)
    print "udp received message:", data
    udp6_socket.sendto(data, addr)
    print "udp echo message to client:", data
