#pragma once

#include "Sample.h"

#include <map>
#include <comdef.h>
#include <wbemidl.h>

DEF_SAMPLE(WMIProvider)
{
  #if defined(VU_WMI_ENABLED)

  // Example 1
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

  // Example 2
  // Get Type of Disks

  std::map<vu::eDiskType, std::wstring> types;
  types[vu::eDiskType::Unspecified] = L"Unspecified";
  types[vu::eDiskType::HDD] = L"HDD";
  types[vu::eDiskType::SSD] = L"SSD";
  types[vu::eDiskType::SCM] = L"SCM";

  wchar_t drives[] = { L'C', L'D', L'E', L'F', L'G', L'?' };
  for (auto drive : drives)
  {
    auto type = vu::GetDiskType(drive);
    std::wcout << drive << " -> " << types[type] << std::endl;
  }

  #endif // VU_WMI_ENABLED

  return vu::VU_OK;
}
