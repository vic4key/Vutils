#pragma once

#include "Sample.h"

DEF_SAMPLE(Others)
{
  std::tstring text;
  std::vector<vu::byte> data = { 0x41, 0x42, 0x43, 0x44, 0x45 };

  vu::crypt_b64encode(data, text);
  std::tcout << text << std::endl;

  data.clear();
  vu::crypt_b64decode(text, data);
  std::cout << data.data() << std::endl;

  data.clear();
  vu::read_file(ts("Test.exe"), data);
  vu::crypt_b64encode(data, text);

  data.clear();
  vu::crypt_b64decode(text, data);
  vu::write_file(ts("Test-B64Decoded.exe"), data);

  return vu::VU_OK;
}
