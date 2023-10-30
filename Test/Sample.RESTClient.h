#pragma once

#include "Sample.h"

DEF_SAMPLE(RESTClient)
{
  #if defined(VU_INET_ENABLED)

  std::tstring header;
  header.append(ts("Accept: */*\r\n"));
  header.append(ts("Accept-Encoding: *\r\n"));
  header.append(ts("Accept-Language: *\r\n"));
  header.append(ts("Content-Type: application/json\r\n"));
  header.append(ts("Connection: keep-alive\r\n"));
  header.append(ts("\r\n"));

  vu::http_response response;
  vu::RESTClient rest_client(vu::protocol_scheme::https, ts("5f9c32b6856f4c00168c7da2.mockapi.io"), 443);

  // get all
  rest_client.get(ts("/api/v1/customers"), response, header);
  std::cout << vu::decode_const_http_status_A(response.status) << std::endl;
  std::cout << response.text << std::endl;

  // create one
  rest_client.post(ts("/api/v1/customers"), response, "{\"name\":\"name 5\",\"phone\":\"phone 5\"}", header);
  std::cout << vu::decode_const_http_status_A(response.status) << std::endl;
  std::cout << response.text << std::endl;

  // get one
  rest_client.get(ts("/api/v1/customers/5"), response, header);
  std::cout << vu::decode_const_http_status_A(response.status) << std::endl;
  std::cout << response.text << std::endl;

  // update one
  rest_client.put(ts("/api/v1/customers/5"), response, "{\"name\":\"name 5-x\",\"phone\":\"phone 5-x\"}", header);
  std::cout << vu::decode_const_http_status_A(response.status) << std::endl;
  std::cout << response.text << std::endl;

  // delete one
  rest_client.del(ts("/api/v1/customers/5"), response, header);
  std::cout << vu::decode_const_http_status_A(response.status) << std::endl;
  std::cout << response.text << std::endl;

  #endif // VU_INET_ENABLED

  return vu::VU_OK;
}
