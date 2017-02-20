#ifndef NAT_H
#define NAT_H

#include <vector>

#include "wyatt_sock.h"
/*
Protocol
first byte in every packet is the message type
message type 0 is a server wanting to listen for new connections just a message type
message type 1 is a client trying to connect to a server, data is an unsigned long the server ip
message type 2 is stop server from listening for connections, just a message type no body
message type 3 is telling a client a server does not exist, just message type no body
message type 4 is an address and port of a server for the client to send to
*/
class NAT
{
  //the socket listening for new connections
  SOCKET sock_;
  //the NATs address
  sockaddr_in addr_;
  //an array of servers that are looking for clients
  std::vector<sockaddr_in> connections_;
  //char array to read data into
  char buffer_[64];
public:
  //opens its port for incoming connections
  NAT(int port);
  ~NAT();
  //checks for new connections, if server adds the server, if client connects client to the server
  //non blocking needs to be called often, like in a while loop
  void Update();
};

#endif