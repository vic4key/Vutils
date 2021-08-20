#pragma once

#include "Sample.h"

DEF_SAMPLE(GlobalUID)
{
  #if defined(VU_GUID_ENABLED)

  for (auto& e : std::vector<int>(5))
  {
    vu::CGUID guid(true);
    std::tcout << guid.as_string() << std::endl;
    std::tcout << vu::CGUID::to_string(vu::CGUID::to_guid(guid.as_string())) << std::endl;
  }

  #endif // VU_GUID_ENABLED

  return vu::VU_OK;
}
