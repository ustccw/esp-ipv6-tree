#!/usr/bin/env python
#-*- coding: utf-8 -*-
# chenwu@espressif.com

import socket

IP_ADDR = "240e:3a1:2074:430:4cd8:1475:ddb5:614a"
TCP_PORT = 59652   # port of magic number (0xE904)

message = "Hello, World!"

print "TCP target IP:", IP_ADDR
print "TCP target port:", TCP_PORT
print "send message:", message

tcp6_socket = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
tcp6_socket.connect((IP_ADDR, TCP_PORT))

tcp6_socket.send(message)

data = tcp6_socket.recv(1460)
print "tcp6 received message from server:", tcp6_socket.getpeername(), "\r\n", data
