#!/usr/bin/env python
#-*- coding: utf-8 -*-
# chenwu@espressif.com

import socket

UDP_IP = "240e:3a1:2074:430:4cd8:1475:ddb5:614a"
UDP_PORT = 59652    # port of magic number (0xE904)

message = "Hello, World!"

print "UDP target IP:", UDP_IP
print "UDP target port:", UDP_PORT
print "message:", message

udp_socket = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
udp_socket.sendto(message, (UDP_IP, UDP_PORT))
