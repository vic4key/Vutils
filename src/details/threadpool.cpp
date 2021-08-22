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

CThreadPool::CThreadPool(size_t n_threads)
{
  if (n_threads == MAX_NTHREADS)
  {
    n_threads = std::thread::hardware_concurrency();
  }

  m_ptr_impl = new Pool(n_threads);
}

CThreadPool::~CThreadPool()
{
  delete m_ptr_impl;
}

void CThreadPool::add_task(FnTask&& fn)
{
  m_ptr_impl->postWork(static_cast<Worker::WorkType>(fn));
}

void CThreadPool::launch()
{
  m_ptr_impl->waitAll();
}

size_t CThreadPool::worker_count() const
{
  return m_ptr_impl->getWorkerCount();
}

size_t CThreadPool::work_queue_count() const
{
  return m_ptr_impl->getWorkQueueCount();
}

size_t CThreadPool::active_worker_count() const
{
  return m_ptr_impl->getActiveWorkerCount();
}

size_t CThreadPool::inactive_worker_count() const
{
  return m_ptr_impl->getInactiveWorkerCount();
}

} // namespace vu