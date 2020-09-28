#!/usr/bin/env python
#-*- coding: utf-8 -*-
# chenwu@espressif.com

import socket

IP_ADDR = "240e:3a1:2074:430:4cd8:1475:ddb5:614a"
TCP_PORT = 59652   # port of magic number (0xE904)

listen6_socket = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
listen6_socket.bind((IP_ADDR, TCP_PORT))
listen6_socket.listen(5)

while True:
    client6_socket, addr = listen6_socket.accept()
    print "Connected from: ", client6_socket.getpeername()
    data = client6_socket.recv(1460)
    print "tcp6 received message:", data
    client6_socket.send(data)
    print "tcp6 echo message to client:", data
