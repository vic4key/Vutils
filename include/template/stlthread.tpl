/**
 * @file   stlthread.tpl
 * @author Vic P.
 * @brief  Template for STL Threading
 */

 /**
  * STLThreadT
  */

enum class return_type
{
  Ok,
  Break,
  Continue,
};

template <typename type_input>
class STLThreadT
{
public:
  STLThreadT(type_input& items, int n_threads = MAX_NTHREADS);
  virtual ~STLThreadT();

  virtual void initialize();
  virtual void launch();

  virtual return_type task(typename type_input::value_type& item, int iteration, int thread_id);

  int threads() const;
  int iterations() const;

protected:
  virtual void execute(int iteration, int thread_id);

protected:
  ThreadPool* m_ptr_thread_pool;
  std::mutex  m_mutex;
  type_input& m_items;
  int m_num_threads;
  int m_num_iterations;
  int m_num_items_per_thread;
};

template <class type_input>
STLThreadT<type_input>::STLThreadT(type_input& items, int n_threads)
  : m_items(items),  m_num_threads(n_threads), m_num_iterations(0), m_ptr_thread_pool(nullptr)
{
  if (m_num_threads == MAX_NTHREADS)
  {
    m_num_threads = std::thread::hardware_concurrency();
  }

  const int n_items = static_cast<int>(m_items.size());

  if (m_num_threads > n_items)
  {
    m_num_threads = n_items;
  }

  m_num_items_per_thread = n_items / m_num_threads;

  m_num_iterations = m_num_threads;
  if (m_items.size() % m_num_threads != 0)
  {
    m_num_iterations += 1; // + 1 for remainder items
  }

  m_ptr_thread_pool = new ThreadPool(m_num_threads);
}

template <class type_input>
STLThreadT<type_input>::~STLThreadT()
{
  if (m_ptr_thread_pool != nullptr)
  {
    delete m_ptr_thread_pool;
  }
}

template <class type_input>
int STLThreadT<type_input>::threads() const
{
  return m_num_threads;
}

template <class type_input>
int STLThreadT<type_input>::iterations() const
{
  return m_num_iterations;
}

template <class type_input>
vu::return_type STLThreadT<type_input>::task(typename type_input::value_type& item, int iteration, int thread_id)
{
  assert(NULL && "This method must be overridden");
  return vu::return_type::Ok;
}

template <class type_input>
void STLThreadT<type_input>::launch()
{
  this->initialize();

  for (int iteration = 0; iteration < m_num_iterations; iteration++)
  {
    m_ptr_thread_pool->add_task([=]()
    {
      std::stringstream ss;
      ss << std::this_thread::get_id();
      int thread_id = std::stoi(ss.str());
      this->execute(iteration, thread_id);
    });
  }

  m_ptr_thread_pool->launch();
}

template <class type_input>
void STLThreadT<type_input>::initialize()
{
  // Override this method to do anything before launching
}

template <class type_input>
void STLThreadT<type_input>::execute(int iteration, int thread_id)
{
  // std::lock_guard<std::mutex> lg(m_mutex); // TODO: Vic. Recheck. Avoid race condition.

  auto num_items = static_cast<int>(m_items.size());

  int start = m_num_items_per_thread * iteration;
  int stop  = m_num_items_per_thread * (iteration + 1) - 1; // - 1 because the index is it_started at 0

  if (stop > num_items - 1)
  {
    stop = num_items - 1;
  }

  auto it_start = std::next(m_items.begin(), start);
  auto it_stop  = std::next(m_items.begin(), stop + 1); // + 1 to iterate to the last item

  // TODO: Vic. Recheck. Must be so faster by non-locking.
  // auto ptr = &m_items[0];
  // for (int i = start; i <= stop; i++)
  // {
  //   auto ret = this->task(ptr[i], iteration, thread_id);

  for (auto it = it_start; it != it_stop; ++it)
  {
    auto ret = this->task(*it, iteration, thread_id);
    if (ret == vu::return_type::Break)
    {
      break;
    }
    else if (ret == vu::return_type::Continue)
    {
      continue;
    }
  }
}