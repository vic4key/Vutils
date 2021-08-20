#pragma once

// https://github.com/vic4key/Async-Socket-Example-MFC

#include "Sample.h"

#if defined(VU_SOCKET_ENABLED)

void fnExampleBinding(const vu::CSocket::TEndPoint& endpoint)
{
  vu::CAsyncSocket server;

  server.on(vu::CAsyncSocket::OPEN, [](vu::CSocket& client) -> void
  {
    printf("\n");
    printf("client %d opened\n", client.get_remote_sai().sin_port);
  });

  server.on(vu::CAsyncSocket::CLOSE, [](vu::CSocket& client) -> void
  {
    printf("client %d closed\n", client.get_remote_sai().sin_port);
  });

  server.on(vu::CAsyncSocket::SEND, [](vu::CSocket& client) -> void
  {
    std::string s = "hello from server";
    client.send(s.data(), int(s.size()));
    printf("client %d send `%s`\n", client.get_remote_sai().sin_port, s.c_str());
  });

  server.on(vu::CAsyncSocket::RECV, [](vu::CSocket& client) -> void
  {
    vu::CBuffer data(KiB);
    client.recv(data);
    printf("client %d recv `%s`\n", client.get_remote_sai().sin_port, data.to_string_A().c_str());
  });

  server.bind(endpoint);
  server.listen();
  server.run();
  server.close();
}

void fnExampleInheritance(const vu::CSocket::TEndPoint& endpoint)
{
  class CAsyncSocketServer : public vu::CAsyncSocket
  {
  public:
    virtual void on_open(vu::CSocket& client)
    {
      printf("\n");
      printf("client %d opened\n", client.get_remote_sai().sin_port);
    }

    virtual void on_close(vu::CSocket& client)
    {
      printf("client %d closed\n", client.get_remote_sai().sin_port);
    }

    virtual void on_send(vu::CSocket& client)
    {
      std::string s = "hello from server";
      client.send(s.data(), int(s.size()));
      printf("client %d send `%s`\n", client.get_remote_sai().sin_port, s.c_str());
    }

    virtual void on_recv(vu::CSocket& client)
    {
      vu::CBuffer data(KiB);
      client.recv(data);
      std::string s(reinterpret_cast<char*>(data.get_ptr_bytes()));
      printf("client %d recv `%s`\n", client.get_remote_sai().sin_port, s.c_str());
    }
  };

  CAsyncSocketServer server;
  server.bind(endpoint);
  server.listen();
  server.run();
  server.close();
}

#endif // VU_SOCKET_ENABLED

DEF_SAMPLE(AsyncSocket)
{
  #if defined(VU_SOCKET_ENABLED)
  // const vu::CSocket::TEndPoint EndPoint("127.0.0.1", 1609);
  // fnExampleBinding(EndPoint);
  // fnExampleInheritance(EndPoint);
  #endif // VU_SOCKET_ENABLED

  return vu::VU_OK;
}
