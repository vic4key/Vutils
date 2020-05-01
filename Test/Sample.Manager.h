#pragma once

#include <cassert>
#include <iostream>
#include <vector>
#include <utility>

#include "Sample.h"

class SampleManager
{
public:
  SampleManager();
  virtual ~SampleManager();

  void add(const std::string& name, Sample* sample);
  void run();

private:
  std::vector<std::pair<Sample*, std::string>> m_samples;
};

SampleManager::SampleManager()
{
  m_samples.clear();
}

SampleManager::~SampleManager()
{
  for (auto& e : m_samples)
  {
    if (e.first != nullptr)
    {
      delete e.first;
    }
  }

  m_samples.clear();
}

void SampleManager::add(const std::string& name, Sample* sample)
{
  if (sample == nullptr || name.empty())
  {
    return;
  }

  m_samples.emplace_back(std::make_pair(sample, name));
}

void SampleManager::run()
{
  for (auto& e : m_samples)
  {
    std::cout << std::endl;
    std::cout << "--- [" << e.second << "] ---" << std::endl;
    e.first->run();
    std::cout << "--- [" << e.second << "] ---" << std::endl;
    std::cout << std::endl;
  }
}

#define VU_SM_INIT() SampleManager samples
#define VU_SM_ADD_SAMPLE(sample) samples.add(#sample, new sample)
#define VU_SM_RUN() samples.run()
