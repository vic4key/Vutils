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

ThreadPool::ThreadPool(size_t n_threads)
{
  if (n_threads == MAX_NTHREADS)
  {
    n_threads = std::thread::hardware_concurrency();
  }

  m_ptr_impl = new Pool(n_threads);
}

ThreadPool::~ThreadPool()
{
  delete m_ptr_impl;
}

void ThreadPool::add_task(fn_task_t&& fn)
{
  m_ptr_impl->postWork(static_cast<Worker::WorkType>(fn));
}

void ThreadPool::launch()
{
  m_ptr_impl->waitAll();
}

size_t ThreadPool::worker_count() const
{
  return m_ptr_impl->getWorkerCount();
}

size_t ThreadPool::work_queue_count() const
{
  return m_ptr_impl->getWorkQueueCount();
}

size_t ThreadPool::active_worker_count() const
{
  return m_ptr_impl->getActiveWorkerCount();
}

size_t ThreadPool::inactive_worker_count() const
{
  return m_ptr_impl->getInactiveWorkerCount();
}

} // namespace vu