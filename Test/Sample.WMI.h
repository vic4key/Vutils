#pragma once

#include "Sample.h"

#if defined(VU_WMI_ENABLED)
#include <map>
#include <comdef.h>
#include <wbemidl.h>
#endif // VU_WMI_ENABLED

DEF_SAMPLE(WMIProvider)
{
  #if defined(VU_WMI_ENABLED)

  // Example 1
  // Get-WmiObject -Class MSFT_PhysicalDisk -Namespace ROOT\Microsoft\Windows\Storage | Select FriendlyName

  vu::WMIProvider wmi;
  wmi.begin(ts("ROOT\\Microsoft\\Windows\\Storage"));
  {
    wmi.query(ts("SELECT * FROM MSFT_PhysicalDisk"), [](IWbemClassObject& object) -> bool
    {
      VARIANT s;
      object.Get(L"FriendlyName", 0, &s, 0, 0);
      std::wcout << s.bstrVal << std::endl;
      return true;
    });
  }
  wmi.end();

  // Example 2
  // Get Type of Disks

  std::map<vu::eDiskType, std::tstring> types;
  types[vu::eDiskType::Unspecified] = ts("Unspecified");
  types[vu::eDiskType::HDD] = ts("HDD");
  types[vu::eDiskType::SSD] = ts("SSD");
  types[vu::eDiskType::SCM] = ts("SCM");
  
  std::tstring drives = ts("CDEFG?");

  for (const auto drive : drives)
  {
    auto type = vu::get_disk_type(drive);
    std::tcout << drive << ts(" -> ") << types[type] << std::endl;
  }

  #endif // VU_WMI_ENABLED

  return vu::VU_OK;
}
