/*******************************************************************************
* File       : wyatt_sock.h
* Author     : Wyatt Lavigueure
* Email      : wyatt.l@digipen.edu
* Team Name  : Lost Rock
* Game Name  : Litho Sphere

*
* Copyright 1996-2016, DigiPen Institute of Technology(USA).All rights reserved.
*******************************************************************************/
#define EBLOCK -6969
#define MAXPACKET 548
#ifndef NOSOCKET
#ifndef WYATT_SOCK_H
#define WYATT_SOCK_H
#include <stdlib.h>  /*exit, calloc, free*/
#ifdef _WIN32
#include <WinSock2.h> /*sockets*/
#else
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <unistd.h>
typedef int SOCKET;
#define SD_SEND SHUT_WR
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#endif

void SetNonBlocking(SOCKET sock);
int GetError();
void Close(SOCKET sock, bool now);
int Send(SOCKET sock, const char* buffer, unsigned bytes, sockaddr_in* dest);
int Receive(SOCKET sock, char* buffer, unsigned maxBytes);
int Receive(SOCKET sock, char* buffer, unsigned maxBytes, sockaddr_in *addr);
int Bind(SOCKET sock, sockaddr_in* addr);
sockaddr_in* CreateAddress(const char* ip, int port);
void CreateAddress(const char* ip, int port, sockaddr_in *addr);
SOCKET CreateSocket(int protocol);
int Connect(SOCKET sock, sockaddr_in* address);
int SendTCP(SOCKET sock, const char* buffer, unsigned bytes);
int ReceiveTCP(SOCKET sock, char* buffer, unsigned maxBytes);
int Init();
void Deinit();
#endif
#endif