#pragma once

#include "Sample.h"

#include <comdef.h>
#include <wbemidl.h>

DEF_SAMPLE(WMIProvider)
{
  #if defined(VU_WMI_ENABLED)

  // Get-WmiObject -Class MSFT_PhysicalDisk -Namespace ROOT\Microsoft\Windows\Storage | Select FriendlyName

  vu::CWMIProvider WMI;
  WMI.Begin(L"ROOT\\Microsoft\\Windows\\Storage");
  {
    WMI.Query(L"SELECT * FROM MSFT_PhysicalDisk", [](IWbemClassObject& object) -> bool
    {
      VARIANT s;
      object.Get(L"FriendlyName", 0, &s, 0, 0);
      std::wcout << s.bstrVal << std::endl;
      return true;
    });
  }
  WMI.End();

  #endif // VU_WMI_ENABLED

  return vu::VU_OK;
}
