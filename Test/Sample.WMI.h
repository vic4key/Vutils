#pragma once

#include "Sample.h"

#include <comdef.h>
#include <wbemidl.h>

DEF_SAMPLE(WMIProvider)
{
  vu::CWMIProvider WMI;
  WMI.Begin(L"ROOT\\microsoft\\windows\\storage");
  {
    WMI.Query(L"SELECT * FROM MSFT_PhysicalDisk", [](IWbemClassObject& object) -> bool
    {
      VARIANT s;
      object.Get(L"FriendlyName", 0, &s, 0, 0);
      std::cout << _bstr_t(s.bstrVal) << std::endl;
      return true;
    });
  }
  WMI.End();

  return vu::VU_OK;
}
