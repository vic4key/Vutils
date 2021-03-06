/**
 * @file   threadpool.cpp
 * @author Vic P.
 * @brief  Implementation for Thread Pool
 */

#include "Vutils.h"
#include "defs.h"

#include VU_3RD_INCL(TP11/include/threadpool11/threadpool11.h)

using namespace threadpool11;

namespace vu
{

CThreadPool::CThreadPool(size_t nthreads)
{
  if (nthreads == MAX_NTHREADS)
  {
    nthreads = std::thread::hardware_concurrency();
  }

  m_pTP = new Pool(nthreads);
}

CThreadPool::~CThreadPool()
{
  delete m_pTP;
}

void CThreadPool::AddTask(FnTask&& fn)
{
  m_pTP->postWork(static_cast<Worker::WorkType>(fn));
}

void CThreadPool::Launch()
{
  m_pTP->waitAll();
}

size_t CThreadPool::WorkerCount() const
{
  return m_pTP->getWorkerCount();
}

size_t CThreadPool::WorkQueueCount() const
{
  return m_pTP->getWorkQueueCount();
}

size_t CThreadPool::ActiveWorkerCount() const
{
  return m_pTP->getActiveWorkerCount();
}

size_t CThreadPool::InactiveWorkerCount() const
{
  return m_pTP->getInactiveWorkerCount();
}

} // namespace vu