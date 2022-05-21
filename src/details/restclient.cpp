/**
 * @file   restclient.cpp
 * @author Vic P.
 * @brief  Implementation for REST Client
 */

#include "Vutils.h"
#include "defs.h"

#include VU_3RD_INCL(WHW/WinHttpWrapper.h)

namespace vu
{

RESTClientA::RESTClientA(
  const scheme_t scheme,
  const std::string& host,
  const int port,
  const std::string& user_agent,
  const std::string& server_user,
  const std::string& server_pass,
  const std::string& proxy_user,
  const std::string& proxy_pass)
{
  m_ptr_impl = new RESTClientW(
    scheme,
    to_string_W(host),
    port,
    to_string_W(user_agent),
    to_string_W(proxy_user),
    to_string_W(proxy_pass),
    to_string_W(server_user),
    to_string_W(server_pass)
  );
}

RESTClientA::~RESTClientA()
{
  delete m_ptr_impl;
}

bool RESTClientA::get(
  const std::string& endpoint, http_response_t& response, const std::string& header)
{
  return m_ptr_impl->get(to_string_W(endpoint), response, to_string_W(header));
}

bool RESTClientA::del(
  const std::string& endpoint, http_response_t& response, const std::string& header)
{
  return m_ptr_impl->del(to_string_W(endpoint), response, to_string_W(header));
}

bool RESTClientA::put(
  const std::string& endpoint, http_response_t& response, const std::string& body, const std::string& header)
{
  return m_ptr_impl->put(to_string_W(endpoint), response, body, to_string_W(header));
}

bool RESTClientA::post(
  const std::string& endpoint, http_response_t& response, const std::string& body, const std::string& header)
{
  return m_ptr_impl->post(to_string_W(endpoint), response, body, to_string_W(header));
}

RESTClientW::RESTClientW(
  const scheme_t scheme,
  const std::wstring& host,
  const int port,
  const std::wstring& user_agent,
  const std::wstring& server_user,
  const std::wstring& server_pass,
  const std::wstring& proxy_user,
  const std::wstring& proxy_pass)
{
  m_ptr_impl = new HttpRequest(
    host,
    port,
    scheme == scheme_t::https,
    user_agent,
    proxy_user,
    proxy_pass,
    server_user,
    server_pass
  );
}

RESTClientW::~RESTClientW()
{
  delete m_ptr_impl;
}

bool RESTClientW::get(
  const std::wstring& endpoint, http_response_t& response, const std::wstring& header)
{
  response.clear();
  return m_ptr_impl->Get(endpoint, header, response);
}

bool RESTClientW::del(
  const std::wstring& endpoint, http_response_t& response, const std::wstring& header)
{
  response.clear();
  return m_ptr_impl->Delete(endpoint, header, "", response);
}

bool RESTClientW::put(
  const std::wstring& endpoint, http_response_t& response, const std::string& body, const std::wstring& header)
{
  response.clear();
  return m_ptr_impl->Put(endpoint, header, body, response);
}

bool RESTClientW::post(
  const std::wstring& endpoint, http_response_t& response, const std::string& body, const std::wstring& header)
{
  response.clear();
  return m_ptr_impl->Post(endpoint, header, body, response);
}

#define STATUS_DEF(id, name) { id, (char*) # name},

static const struct { ulong code; char* text; } LIST_HTTP_STATUS[] = \
{
  // 1xx - Informational
  STATUS_DEF(100, Continue)
  STATUS_DEF(101, Switching Protocols)
  STATUS_DEF(102, Processing)
  STATUS_DEF(103, Early Hints)
  // 2xx - Successful
  STATUS_DEF(200, OK)
  STATUS_DEF(201, Created)
  STATUS_DEF(202, Accepted)
  STATUS_DEF(203, Non-Authoritative Information)
  STATUS_DEF(204, No Content)
  STATUS_DEF(205, Reset Content)
  STATUS_DEF(206, Partial Content)
  STATUS_DEF(207, Multi-Status)
  STATUS_DEF(208, Already Reported)
  STATUS_DEF(226, IM Used)
  // 3xx - Redirection
  STATUS_DEF(300, Multiple Choices)
  STATUS_DEF(301, Moved Permanently)
  STATUS_DEF(302, Found)
  STATUS_DEF(303, See Other)
  STATUS_DEF(304, Not Modified)
  STATUS_DEF(305, Use Proxy)
  STATUS_DEF(307, Temporary Redirect)
  STATUS_DEF(308, Permanent Redirect)
  // 4xx - Client Error
  STATUS_DEF(400, Bad Request)
  STATUS_DEF(401, Unauthorized)
  STATUS_DEF(402, Payment Required)
  STATUS_DEF(403, Forbidden)
  STATUS_DEF(404, Not Found)
  STATUS_DEF(405, Method Not Allowed)
  STATUS_DEF(406, Not Acceptable)
  STATUS_DEF(407, Proxy Authentication Required)
  STATUS_DEF(408, Request Timeout)
  STATUS_DEF(409, Conflict)
  STATUS_DEF(410, Gone)
  STATUS_DEF(411, Length Required)
  STATUS_DEF(412, Precondition Failed)
  STATUS_DEF(413, Content Too Large)
  STATUS_DEF(414, URI Too Long)
  STATUS_DEF(415, Unsupported Media Type)
  STATUS_DEF(416, Range Not Satisfiable)
  STATUS_DEF(417, Expectation Failed)
  STATUS_DEF(418, I''m a teapot)
  STATUS_DEF(421, Misdirected Request)
  STATUS_DEF(422, Unprocessable Content)
  STATUS_DEF(423, Locked)
  STATUS_DEF(424, Failed Dependency)
  STATUS_DEF(425, Too Early)
  STATUS_DEF(426, Upgrade Required)
  STATUS_DEF(428, Precondition Required)
  STATUS_DEF(429, Too Many Requests)
  STATUS_DEF(431, Request Header Fields Too Large)
  STATUS_DEF(451, Unavailable For Legal Reasons)
  // 5xx - Server Error
  STATUS_DEF(500, Internal Server Error)
  STATUS_DEF(501, Not Implemented)
  STATUS_DEF(502, Bad Gateway)
  STATUS_DEF(503, Service Unavailable)
  STATUS_DEF(504, Gateway Timeout)
  STATUS_DEF(505, HTTP Version Not Supported)
  STATUS_DEF(506, Variant Also Negotiates)
  STATUS_DEF(507, Insufficient Storage)
  STATUS_DEF(508, Loop Detected)
  STATUS_DEF(510, Not Extended)
  STATUS_DEF(511, Network Authentication Required)
};

std::string vuapi decode_http_status_A(const ulong code)
{
  std::string result = "<unknown>";

  for (auto e : LIST_HTTP_STATUS)
  {
    if (e.code == code)
    {
      result = e.text;
      break;
    }
  }

  return result;
}

std::wstring vuapi decode_http_status_W(const ulong code)
{
  return to_string_W(decode_http_status_A(code));
}

} // namespace vu