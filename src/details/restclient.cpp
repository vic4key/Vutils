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

#ifdef VU_INET_ENABLED

RESTClientA::RESTClientA(
  const protocol_scheme scheme,
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
  const std::string& endpoint, http_response& response, const std::string& header)
{
  return m_ptr_impl->get(to_string_W(endpoint), response, to_string_W(header));
}

bool RESTClientA::del(
  const std::string& endpoint, http_response& response, const std::string& header)
{
  return m_ptr_impl->del(to_string_W(endpoint), response, to_string_W(header));
}

bool RESTClientA::put(
  const std::string& endpoint, http_response& response, const std::string& body, const std::string& header)
{
  return m_ptr_impl->put(to_string_W(endpoint), response, body, to_string_W(header));
}

bool RESTClientA::post(
  const std::string& endpoint, http_response& response, const std::string& body, const std::string& header)
{
  return m_ptr_impl->post(to_string_W(endpoint), response, body, to_string_W(header));
}

RESTClientW::RESTClientW(
  const protocol_scheme scheme,
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
    scheme == protocol_scheme::https,
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
  const std::wstring& endpoint, http_response& response, const std::wstring& header)
{
  response.clear();
  return m_ptr_impl->Get(endpoint, header, response);
}

bool RESTClientW::del(
  const std::wstring& endpoint, http_response& response, const std::wstring& header)
{
  response.clear();
  return m_ptr_impl->Delete(endpoint, header, "", response);
}

bool RESTClientW::put(
  const std::wstring& endpoint, http_response& response, const std::string& body, const std::wstring& header)
{
  response.clear();
  return m_ptr_impl->Put(endpoint, header, body, response);
}

bool RESTClientW::post(
  const std::wstring& endpoint, http_response& response, const std::string& body, const std::wstring& header)
{
  response.clear();
  return m_ptr_impl->Post(endpoint, header, body, response);
}

#endif // VU_INET_ENABLED

} // namespace vu