/**
 * @file   stlthread.tpl
 * @author Vic P.
 * @brief  Template for STL Threading
 */

 /**
  * CSTLThreadT
  */

enum eReturn
{
  Ok,
  Break,
  Continue,
};

template <typename TypeInput>
class CSTLThreadT
{
public:
  CSTLThreadT(TypeInput& items, int nThreads = MAX_NTHREADS);
  virtual ~CSTLThreadT();

  virtual void Initialize();
  virtual void Launch();

  virtual const eReturn Task(typename TypeInput::value_type& item, int iteration, int thread_id);

  int Threads() const;
  int Iterations() const;

protected:
  virtual void Execute(int iteration, int threadID);

protected:
  CThreadPool* m_ptr_threadpool;
  std::mutex m_mutex;
  TypeInput& m_items;
  int m_num_threads;
  int m_num_iterations;
  int m_num_items_per_thread;
};

template <class TypeInput>
CSTLThreadT<TypeInput>::CSTLThreadT(TypeInput& items, int nThreads)
  : m_items(items),  m_num_threads(nThreads), m_num_iterations(0), m_ptr_threadpool(nullptr)
{
  if (m_num_threads == MAX_NTHREADS)
  {
    m_num_threads = std::thread::hardware_concurrency();
  }

  if (m_num_threads > m_items.size())
  {
    m_num_threads = static_cast<int>(m_items.size());
  }

  m_num_items_per_thread = static_cast<int>(m_items.size() / m_num_threads);

  m_num_iterations = m_num_threads;
  if (m_items.size() % m_num_threads != 0)
  {
    m_num_iterations += 1; // + 1 for remainder items
  }

  m_ptr_threadpool = new CThreadPool(m_num_threads);
}

template <class TypeInput>
CSTLThreadT<TypeInput>::~CSTLThreadT()
{
  if (m_ptr_threadpool != nullptr)
  {
    delete m_ptr_threadpool;
  }
}

template <class TypeInput>
int CSTLThreadT<TypeInput>::Threads() const
{
  return m_num_threads;
}

template <class TypeInput>
int CSTLThreadT<TypeInput>::Iterations() const
{
  return m_num_iterations;
}

template <class TypeInput>
const eReturn CSTLThreadT<TypeInput>::Task(
  typename TypeInput::value_type& item, int iteration, int thread_id)
{
  assert(NULL && "This method must be overridden");
  return eReturn::Ok;
}

template <class TypeInput>
void CSTLThreadT<TypeInput>::Launch()
{
  this->Initialize();

  for (int iteration = 0; iteration < m_num_iterations; iteration++)
  {
    m_ptr_threadpool->AddTask([=]()
    {
      std::stringstream ss;
      ss << std::this_thread::get_id();
      int thread_id = std::stoi(ss.str());

      this->Execute(iteration, thread_id);
    });
  }

  m_ptr_threadpool->Launch();
}

template <class TypeInput>
void CSTLThreadT<TypeInput>::Initialize()
{
  // Override this method to do anything before launching
}

template <class TypeInput>
void CSTLThreadT<TypeInput>::Execute(int iteration, int thread_id)
{
  // std::lock_guard<std::mutex> lg(m_mutex); // TODO: Vic. Recheck. Avoid race condition.

  auto nItems = static_cast<int>(m_items.size());

  int start = m_num_items_per_thread * iteration;
  int stop  = m_num_items_per_thread * (iteration + 1) - 1; // - 1 because the index is itstarted at 0

  if (stop > nItems - 1)
  {
    stop = nItems - 1;
  }

  auto itstart = std::next(m_items.begin(), start);
  auto itstop  = std::next(m_items.begin(), stop + 1); // + 1 to iterate to the last item

  // TODO: Vic. Recheck. Must be so faster by non-locking.
  // auto ptr = &m_items[0];
  // for (int i = start; i <= stop; i++)
  // {
  //   auto ret = this->Task(ptr[i], iteration, thread_id);

  for (auto it = itstart; it != itstop; ++it)
  {
    auto ret = this->Task(*it, iteration, thread_id);
    if (ret == eReturn::Break)
    {
      break;
    }
    else if (ret == eReturn::Continue)
    {
      continue;
    }
  }
}