#pragma once

#include "Sample.h"

DEF_SAMPLE(Singleton)
{
  class Service : public vu::SingletonT<Service>
  {
  public:
    void start() {}
    void stop() {}
  };

  Service::instance().start();
  Service::instance().stop();

  return vu::VU_OK;
}
