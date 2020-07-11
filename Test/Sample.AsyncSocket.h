#pragma once

#include "Sample.h"

class CAsyncSocketServer : public vu::CAsyncSocket
{
public:
  CAsyncSocketServer();
  virtual ~CAsyncSocketServer();

  virtual void OnOpen(vu::CSocket& client);
  virtual void OnClose(vu::CSocket& client);
  virtual void OnSend(vu::CSocket& client);
  virtual void OnRecv(vu::CSocket& client);
};

CAsyncSocketServer::CAsyncSocketServer() : vu::CAsyncSocket()
{
}

CAsyncSocketServer::~CAsyncSocketServer()
{
}

void CAsyncSocketServer::OnOpen(vu::CSocket& client)
{
  printf("\n");
  printf("client %d connected\n", client.GetRemoteSAI().sin_port);
}

void CAsyncSocketServer::OnSend(vu::CSocket& client)
{
  std::string s = "hello from server";
  client.Send(s.data(), int(s.length()));
  printf("client %d send `%s`\n", client.GetRemoteSAI().sin_port, s.c_str());
}

void CAsyncSocketServer::OnRecv(vu::CSocket& client)
{
  vu::CBuffer data(KiB);
  client.Recv(data);
  std::string s(reinterpret_cast<char*>(data.GetpBytes()));
  printf("client %d recv `%s`\n", client.GetRemoteSAI().sin_port, s.c_str());
}

void CAsyncSocketServer::OnClose(vu::CSocket& client)
{
  printf("client %d closed\n", client.GetRemoteSAI().sin_port);
}

DEF_SAMPLE(AsyncSocket)
{
  #if defined(VU_SOCKET_ENABLED)

  CAsyncSocketServer server;
  server.Bind("127.0.0.1", 1609);
  server.Listen();
  server.Run();
  server.Close();

  #endif // VU_SOCKET_ENABLED

  return vu::VU_OK;
}
