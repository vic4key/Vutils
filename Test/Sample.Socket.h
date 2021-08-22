#pragma once

#include "Sample.h"

DEF_SAMPLE(Socket)
{
  #if defined(VU_SOCKET_ENABLED)

  std::string req;
  req.append("GET /5MB.zip HTTP/1.1\r\n");
  req.append("Host: ipv4.download.thinkbroadband.com\r\n");
  req.append("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:72.0) Gecko/20100101 Firefox/72.0\r\n");
  req.append("Accept-Language: en-US,en;q=0.5\r\n");
  req.append("Accept-Encoding: gzip, deflate\r\n");
  req.append("Connection: keep-alive\r\n");
  req.append("DNT: 1\r\n");
  req.append("\r\n");

  vu::Socket socket;

  if (socket.connect("ipv4.download.thinkbroadband.com", 80) != vu::VU_OK)
  {
    std::tcout << ts("Socket -> Connect -> Failed") << std::endl;
    return 1;
  }

  std::tcout << ts("Socket -> Connect -> Success") << std::endl;

  if (socket.send(req.data(), int(req.length())) == SOCKET_ERROR)
  {
    std::tcout << ts("Socket -> Send -> Failed") << std::endl;
    return 1;
  }

  // vu::Buffer data;
  // if (socket.recv_all(data) != SOCKET_ERROR)
  // {
  //   data.save_to_file(ts("5MB.bin"));
  // }

  // request to get file

  vu::Buffer response(KiB);
  const auto N = socket.recv(response);
  if (response.empty())
  {
    std::tcout << ts("Socket -> Recv -> Nothing") << std::endl;
    return 1;
  }

  // extract response header & body

  const std::string first_response = response.to_string_A();
  const std::string http_header_sep = std::crlf;
  const std::string http_header_end = http_header_sep + http_header_sep;

  auto l = vu::split_string_A(first_response, http_header_end);
  assert(!l.empty());

  const auto& response_header = l.at(0) + http_header_end;
  std::cout << "Response Header:" << std::endl;
  const auto& headers = vu::split_string_A(response_header, http_header_sep, true);
  for (const auto& e : headers)
  {
    std::cout << std::tab << e << std::endl;
  }

  vu::Buffer buffer(response.get_ptr_bytes() + response_header.length(), N - response_header.length());

  // receive file chunks and append to the file buffer

  vu::Buffer file;

  while (!buffer.empty())
  {
    file.append(buffer);

    buffer.resize(KiB);
    socket.recv(buffer);

    std::cout
      << std::left
      << "Downloaded: "
      << std::setw(15)
      << vu::format_bytes_A(file.get_size())
      << std::cr
      << std::flush;
  }

  std::cout << std::endl;

  // save file buffer to disk

  file.save_to_file(ts("5MB.bin"));

  if (socket.close() != vu::VU_OK)
  {
    std::tcout << ts("Socket -> Close -> Failed") << std::endl;
    return 1;
  }

  std::tcout << ts("Socket -> Close -> Success") << std::endl;

  #endif // VU_SOCKET_ENABLED

  return vu::VU_OK;
}
