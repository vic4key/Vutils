#pragma once

#include <string>

namespace WinHttpWrapper
{
  struct HttpResponse
  {
    HttpResponse()
    {
      clear();
    }

    void clear()
    {
      text   = "";
      header = L"";
      error  = L"";
      status = 0;
    }

    std::string  text;
    std::wstring header;
    std::wstring error;
    DWORD status;
  };
} // WinHttpWrapper