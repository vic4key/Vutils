#pragma once

#include "Sample.h"

DEF_SAMPLE(Others)
{
  std::tstring text = ts("this is an example");
  std::vector<vu::byte> data = { 0x41, 0x42, 0x43, 0x44, 0x45 };

  std::tcout << ts("Crypt - MD5") << std::endl;

  std::tcout << vu::crypt_md5_string(text) << std::endl;
  std::tcout << vu::crypt_md5_buffer(data) << std::endl;
  std::tcout << vu::crypt_md5_file(ts("Test.exe")) << std::endl;

  std::tcout << ts("Crypt - B64") << std::endl;

  text.clear();
  vu::crypt_b64encode(data, text);
  std::tcout << text << std::endl;

  data.clear();
  vu::crypt_b64decode(text, data);
  std::cout << data.data() << std::endl;

  data.clear();
  vu::read_file_binary(ts("Test.exe"), data);
  vu::crypt_b64encode(data, text);

  data.clear();
  vu::crypt_b64decode(text, data);
  vu::write_file_binary(ts("Test-B64Decoded.exe"), data);

  return vu::VU_OK;
}
