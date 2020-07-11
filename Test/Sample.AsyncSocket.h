#pragma once

#include "Sample.h"

class CAsyncSocketServer : public vu::CAsyncSocket
{
public:
  CAsyncSocketServer() : vu::CAsyncSocket() {};
  virtual ~CAsyncSocketServer() {};

  virtual void OnOpen(vu::CSocket& client)
  {
    printf("\n");
    printf("client %d opened\n", client.GetRemoteSAI().sin_port);
  }

  virtual void OnClose(vu::CSocket& client)
  {
    printf("client %d closed\n", client.GetRemoteSAI().sin_port);
  }

  virtual void OnSend(vu::CSocket& client)
  {
    std::string s = "hello from server";
    client.Send(s.data(), int(s.size()));
    printf("client %d send `%s`\n", client.GetRemoteSAI().sin_port, s.c_str());
  }

  virtual void OnRecv(vu::CSocket& client)
  {
    vu::CBuffer data(KiB);
    client.Recv(data);
    std::string s(reinterpret_cast<char*>(data.GetpBytes()));
    printf("client %d recv `%s`\n", client.GetRemoteSAI().sin_port, s.c_str());
  }
};

DEF_SAMPLE(AsyncSocket)
{
  #if defined(VU_SOCKET_ENABLED)

  const vu::CSocket::TEndPoint EndPoint("127.0.0.1", 1609);

  /*// Example 1 : With derived class and their override methods

  CAsyncSocketServer server;
  server.Bind(EndPoint);
  server.Listen();
  server.Run();
  server.Close();*/

  // Example 2 : With base class and mapping lambda functions

  vu::CAsyncSocket server;

  server.On(vu::CAsyncSocket::eFnType::OPEN, [](vu::CSocket& client) -> void
  {
    printf("\n");
    printf("client %d opened\n", client.GetRemoteSAI().sin_port);
  });

  server.On(vu::CAsyncSocket::eFnType::CLOSE, [](vu::CSocket& client) -> void
  {
      printf("client %d closed\n", client.GetRemoteSAI().sin_port);
  });

  server.On(vu::CAsyncSocket::eFnType::SEND, [](vu::CSocket& client) -> void
  {
    std::string s = "hello from server";
    client.Send(s.data(), int(s.size()));
    printf("client %d send `%s`\n", client.GetRemoteSAI().sin_port, s.c_str());
  });

  server.On(vu::CAsyncSocket::eFnType::RECV, [](vu::CSocket& client) -> void
  {
    vu::CBuffer data(KiB);
    client.Recv(data);
    std::string s(reinterpret_cast<char*>(data.GetpBytes()));
    printf("client %d recv `%s`\n", client.GetRemoteSAI().sin_port, s.c_str());
  });

  server.Bind(EndPoint);
  server.Listen();
  server.Run();
  server.Close();

  #endif // VU_SOCKET_ENABLED

  return vu::VU_OK;
}
