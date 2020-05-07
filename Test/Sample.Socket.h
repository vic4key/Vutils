#pragma once

#include "Sample.h"

DEF_SAMPLE(Socket)
{
  const std::string REQ_HOST = "ipv4.download.thinkbroadband.com";
  std::string REQ_CONTENT;
  REQ_CONTENT.clear();
  REQ_CONTENT.append("GET /5MB.zip HTTP/1.1\r\n");
  REQ_CONTENT.append("Host: ipv4.download.thinkbroadband.com\r\n");
  REQ_CONTENT.append("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:72.0) Gecko/20100101 Firefox/72.0\r\n");
  REQ_CONTENT.append("Accept-Language: en-US,en;q=0.5\r\n");
  REQ_CONTENT.append("Accept-Encoding: gzip, deflate\r\n");
  REQ_CONTENT.append("Connection: keep-alive\r\n");
  REQ_CONTENT.append("DNT: 1\r\n");
  REQ_CONTENT.append("\r\n");

  vu::CSocket socket(vu::SAF_INET, vu::ST_STREAM);

  if (socket.Connect(REQ_HOST, 80) != vu::VU_OK)
  {
    std::tcout << _T("Socket -> Connect -> Failed") << std::endl;
    return 1;
  }

  std::tcout << _T("Socket -> Connect -> Success") << std::endl;

  if (socket.Send(REQ_CONTENT.data(), int(REQ_CONTENT.length())) == SOCKET_ERROR)
  {
    std::tcout << _T("Socket -> Send -> Failed") << std::endl;
    return 1;
  }

  vu::CBinary reponse(1024);
  auto N = socket.Recv(reponse);
  assert(N > 0);

  const std::string FirstResponse = (char*)reponse.GetpData();
  const std::string HttpHeaderEnd = "\x0D\x0A\x0D\x0A";
  const std::string HttpHeaderSep = "\x0D\x0A";

  auto l = vu::SplitStringA(FirstResponse, HttpHeaderEnd);
  assert(!l.empty());

  const auto& ResponseHeader = l.at(0) + HttpHeaderEnd;
  std::cout << "Response Header:" << std::endl;
  const auto& headers = vu::SplitStringA(ResponseHeader, HttpHeaderSep, true);
  for (auto& e : headers)
  {
    std::cout << "\t" << e << std::endl;
  }

  reponse.SetpData(
    (char*)reponse.GetpData() + ResponseHeader.length(),
    N - (int)ResponseHeader.length()
  );

  vu::CFileSystemA file("5MB.zip", vu::eFSModeFlags::FM_CREATEALWAY);
  assert(file.IsReady());
  file.Write(reponse.GetpData(), reponse.GetUsedSize());

  vu::IResult nRecvBytes = 0;
  do
  {
    N = socket.Recv(reponse);
    if (N > 0)
    {
      file.Write(reponse.GetpData(), reponse.GetUsedSize());
      nRecvBytes += N;
      std::cout
        << std::left
        << "Downloaded: "
        << std::setw(15)
        << vu::FormatBytesA(nRecvBytes)
        << '\r'
        << std::flush;
    }
  } while (N > 0);

  std::cout << std::endl;

  if (!socket.Close())
  {
    std::tcout << _T("Socket -> Close -> Failed") << std::endl;
    return 1;
  }

  std::tcout << _T("Socket -> Close -> Success") << std::endl;

  return vu::VU_OK;
}
