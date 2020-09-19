#pragma once

#include "Sample.h"

DEF_SAMPLE(Singleton)
{
  class CService : public vu::CSingletonT<CService>
  {
  public:
    void Start() {}
    void Stop() {}
  };

  CService::Instance().Start();
  CService::Instance().Stop();

  return vu::VU_OK;
}
