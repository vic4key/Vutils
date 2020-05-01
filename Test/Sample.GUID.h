#pragma once

#include "Sample.h"

DEF_SAMPLE(GlobalUID)
{
  for (auto& e : std::vector<int>(5))
  {
    vu::CGUID guid(true);
    std::tcout << guid.AsString() << std::endl;
    std::tcout << vu::CGUID::ToString(vu::CGUID::ToGUID(guid.AsString())) << std::endl;
  }

  return vu::VU_OK;
}
