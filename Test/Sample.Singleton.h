#pragma once

#include "Sample.h"

DEF_SAMPLE(Singleton)
{
  class CDog : public vu::CSingletonT<CDog>
  {
  public:
    void Speak()
    {
      std::cout << VU_FUNC_INFO << std::endl;
    };
  };

  class CCat : public vu::CSingletonT<CCat>
  {
  public:
    void Speak()
    {
      std::cout << VU_FUNC_INFO << std::endl;
    };
  };

  CDog::Instance().Speak();
  CCat::Instance().Speak();

  return vu::VU_OK;
}
