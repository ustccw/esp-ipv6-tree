#!/usr/bin/env python
#-*- coding: utf-8 -*-
# chenwu@espressif.com

import socket

IP_ADDR = "240e:3a1:2074:430:4cd8:1475:ddb5:614a"
UDP_PORT = 59652    # port of magic number (0xE904)

message = "Hello, World!"

print "UDP target IP:", IP_ADDR
print "UDP target port:", UDP_PORT
print "message:", message

udp6_socket = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
udp6_socket.sendto(message, (IP_ADDR, UDP_PORT))

data, addr = udp6_socket.recvfrom(1460)
print "udp received message from server:", data
