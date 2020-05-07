#pragma once

#include "Sample.h"

DEF_SAMPLE(Math)
{
  std::cout << "GCD : " << vu::GCD(3, 9, 27, 81) << std::endl;
  std::cout << "LCM : " << vu::LCM(3, 9, 27, 81) << std::endl;

  return vu::VU_OK;
}
