#pragma once

// https://github.com/vic4key/Async-Socket-Example-MFC

#include "Sample.h"

#if defined(VU_SOCKET_ENABLED)

void example_binding(const vu::Socket::sEndPoint& endpoint)
{
  vu::AsyncSocket server;

  server.on(vu::AsyncSocket::OPEN, [](vu::Socket& client) -> void
  {
    printf("\n");
    printf("client %d opened\n", client.get_remote_sai().sin_port);
  });

  server.on(vu::AsyncSocket::CLOSE, [](vu::Socket& client) -> void
  {
    printf("client %d closed\n", client.get_remote_sai().sin_port);
  });

  server.on(vu::AsyncSocket::SEND, [](vu::Socket& client) -> void
  {
    std::string s = "hello from server";
    client.send(s.data(), int(s.size()));
    printf("client %d send `%s`\n", client.get_remote_sai().sin_port, s.c_str());
  });

  server.on(vu::AsyncSocket::RECV, [](vu::Socket& client) -> void
  {
    vu::Buffer data(KiB);
    client.recv(data);
    printf("client %d recv `%s`\n", client.get_remote_sai().sin_port, data.to_string_A().c_str());
  });

  server.bind(endpoint);
  server.listen();
  server.run();
  server.close();
}

void example_inheritance(const vu::Socket::sEndPoint& endpoint)
{
  class CAsyncSocketServer : public vu::AsyncSocket
  {
  public:
    virtual void on_open(vu::Socket& client)
    {
      printf("\n");
      printf("client %d opened\n", client.get_remote_sai().sin_port);
    }

    virtual void on_close(vu::Socket& client)
    {
      printf("client %d closed\n", client.get_remote_sai().sin_port);
    }

    virtual void on_send(vu::Socket& client)
    {
      std::string s = "hello from server";
      client.send(s.data(), int(s.size()));
      printf("client %d send `%s`\n", client.get_remote_sai().sin_port, s.c_str());
    }

    virtual void on_recv(vu::Socket& client)
    {
      vu::Buffer data(KiB);
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
  // const vu::Socket::sEndPoint endpoint("127.0.0.1", 1609);
  // example_binding(endpoint);
  // example_inheritance(endpoint);
  #endif // VU_SOCKET_ENABLED

  return vu::VU_OK;
}
