#pragma once

#include "Sample.h"

DEF_SAMPLE(UIDGlobal)
{
  #if defined(VU_GUID_ENABLED)

  for (auto& e : std::vector<int>(5))
  {
    vu::UIDGlobal guid(true);
    std::tcout << guid.as_string() << std::endl;
    std::tcout << vu::UIDGlobal::to_string(vu::UIDGlobal::to_guid(guid.as_string())) << std::endl;
  }

  #endif // VU_GUID_ENABLED

  return vu::VU_OK;
}
