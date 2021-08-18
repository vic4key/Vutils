#pragma once

#include "Sample.h"

DEF_SAMPLE(Socket)
{
  #if defined(VU_SOCKET_ENABLED)

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

  vu::CSocket socket;

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

  // vu::CBuffer data;
  // if (socket.Recvall(data) != SOCKET_ERROR)
  // {
  //   data.SaveAsFile(_T("5MB.bin"));
  // }

  // request to get file

  vu::CBuffer response(KiB);
  const auto N = socket.Recv(response);
  if (response.Empty())
  {
    std::tcout << _T("Socket -> Recv -> Nothing") << std::endl;
    return 1;
  }

  // extract response header & body

  const std::string FirstResponse = response.ToStringA();
  const std::string HttpHeaderSep = std::crlf;
  const std::string HttpHeaderEnd = HttpHeaderSep + HttpHeaderSep;

  auto l = vu::split_string_A(FirstResponse, HttpHeaderEnd);
  assert(!l.empty());

  const auto& ResponseHeader = l.at(0) + HttpHeaderEnd;
  std::cout << "Response Header:" << std::endl;
  const auto& headers = vu::split_string_A(ResponseHeader, HttpHeaderSep, true);
  for (const auto& e : headers)
  {
    std::cout << std::tab << e << std::endl;
  }

  vu::CBuffer buffer(response.GetpBytes() + ResponseHeader.length(), N - ResponseHeader.length());

  // receive file chunks and append to the file buffer

  vu::CBuffer file;

  while (!buffer.Empty())
  {
    file.Append(buffer);

    buffer.Resize(KiB);
    socket.Recv(buffer);

    std::cout
      << std::left
      << "Downloaded: "
      << std::setw(15)
      << vu::format_bytes_A(file.GetSize())
      << std::cr
      << std::flush;
  }

  std::cout << std::endl;

  // save file buffer to disk

  file.SaveAsFile(_T("5MB.bin"));

  if (socket.Close() != vu::VU_OK)
  {
    std::tcout << _T("Socket -> Close -> Failed") << std::endl;
    return 1;
  }

  std::tcout << _T("Socket -> Close -> Success") << std::endl;

  #endif // VU_SOCKET_ENABLED

  return vu::VU_OK;
}
