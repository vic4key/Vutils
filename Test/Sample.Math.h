#pragma once

#include "Sample.h"

class Math : public Sample
{
public:
  virtual vu::IResult run();
};

vu::IResult Math::run()
{
  std::cout << "GCD : " << vu::GCD(3, 9, 27, 81) << std::endl;
  std::cout << "LCM : " << vu::LCM(3, 9, 27, 81) << std::endl;

  return vu::VU_OK;
}
