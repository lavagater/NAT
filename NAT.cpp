#include "NAT.h"
#include <time.h>

NAT::NAT(int port)
{
  //windows init, on linux this does nothing
  Init();
  //create our address
  CreateAddress(0, port, &addr_);
  //create the socket
  sock_ = CreateSocket(IPPROTO_UDP);
  //bind our socket to our address
  Bind(sock_, &addr_);
  //sets the socket as non blocking
  SetNonBlocking(sock_);
}
NAT::~NAT()
{
  //windows only functions, linux can ignore
  Deinit();
}
void NAT::Update()
{
  unsigned current_time = clock();
  //remove a dead connection if there is one
  int connection_to_remove = -1;
  //update times
  for (unsigned i = 0; i < connections_.size(); ++i)
  {
    if ((current_time - start_times_[i]) / CLOCKS_PER_SEC > PING_FREQUENCY)
    {
      timers_[i] -= PING_FREQUENCY;
      if (timers_[i] <= 0)
      {
        connection_to_remove = i;
        continue;
      }
      Send(sock_, buffer_, 1, &connections_[i]);
      start_times_[i] = current_time;
    }
  }
  if (connection_to_remove != -1)
  {
    //remove them from arrays
    connections_.erase(connections_.begin() + connection_to_remove);
    timers_.erase(timers_.begin() + connection_to_remove);
    start_times_.erase(start_times_.begin() + connection_to_remove);
  }
  sockaddr_in new_client;
  int n = Receive(sock_, buffer_, 64, &new_client);
  //nothing read then we done
  if (n < 0)
  {
    return;
  }
  //first byte sent is the message type
  switch (buffer_[0])
  {
  case 0: //0 = server start listening
  {
    //check if the connection already exists
    bool exists = false;
    for (unsigned i = 0; i < connections_.size(); ++i)
    {
      //make sure same ip and port
      if (connections_[i].sin_addr.s_addr ==
        new_client.sin_addr.s_addr &&
        connections_[i].sin_port == new_client.sin_port)
      {
        break;
      }
    }
    if (exists == false)
    {
      //add the connection
      connections_.push_back(new_client);
      timers_.push_back(CONNECTION_TIME);
      start_times_.push_back(current_time);
    }
    break;
  }
  case 1://1 = client connecting to a server
  {
    //check to make sure they sent enough data
    //the message should be the ip address of the server they want to connect to
    if (n - 1 < sizeof(new_client.sin_addr.s_addr))
    {
      break;
    }
    //find the server in our connections
    unsigned i = 0;
    while (i < connections_.size())
    {
      if (connections_[i].sin_addr.s_addr == *reinterpret_cast<unsigned long*>(buffer_ + 1))
      {
        //found the server
        break;
      }
      i += 1;
    }
    if (i >= connections_.size())
    {
      //we did not find the server tell the client they dont exist
      buffer_[0] = 3;
      Send(sock_, buffer_, 1, &new_client);
      break;
    }
    //send back to the client the address and port to connect to
    buffer_[0] = 4;
    *reinterpret_cast<sockaddr_in*>(buffer_ + 1) = connections_[i];
    Send(sock_, buffer_, sizeof(sockaddr_in) + 1, &new_client);
    //send to the sever as well
    *reinterpret_cast<sockaddr_in*>(buffer_ + 1) = new_client;
    Send(sock_, buffer_, sizeof(sockaddr_in) + 1, &connections_[i]);
    break;
  }
  case 2://2 = server stop listening
  {
    for (unsigned i = 0; i < connections_.size(); ++i)
    {
      //make sure same ip and port
      if (connections_[i].sin_addr.s_addr ==
        new_client.sin_addr.s_addr &&
        connections_[i].sin_port == new_client.sin_port)
      {
        connections_.erase(connections_.begin() + i);
        break;
      }
    }
    break;
  }
  case 5:
  {
    for (unsigned i = 0; i < connections_.size(); ++i)
    {
      //find the index of the connections
      if (connections_[i].sin_addr.s_addr ==
        new_client.sin_addr.s_addr &&
        connections_[i].sin_port == new_client.sin_port)
      {
        timers_[i] = CONNECTION_TIME;
        start_times_[i] = current_time;
        break;
      }
    }
  }
  }
}