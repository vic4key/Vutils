#pragma once

#include "Sample.h"

DEF_SAMPLE(Singleton)
{
  class CService : public vu::SingletonT<CService>
  {
  public:
    void Start() {}
    void Stop() {}
  };

  CService::instance().Start();
  CService::instance().Stop();

  return vu::VU_OK;
}
