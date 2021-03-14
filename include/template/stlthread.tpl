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
  CSTLThreadT(const TypeInput& items, int nThreads = MAX_NTHREADS);
  virtual ~CSTLThreadT();

  virtual void Initialize();
  virtual void Launch();

  virtual const eReturn Task(typename TypeInput::value_type& item, int iteration, int threadid);

  int Threads() const;
  int Iterations() const;

protected:
  virtual void Execute(int iteration, int threadID);

protected:
  CThreadPool* m_pTP;
  // std::mutex m_Mutex;
  int m_nThreads;
  int m_nIterations;
  int m_nItemsPerThread;
  const TypeInput& m_Items;
};

template <class TypeInput>
CSTLThreadT<TypeInput>::CSTLThreadT(const TypeInput& items, int nThreads)
  : m_Items(items),  m_nThreads(nThreads), m_nIterations(0)
{
  if (m_nThreads == MAX_NTHREADS)
  {
    m_nThreads = std::thread::hardware_concurrency();
  }

  if (m_nThreads > m_Items.size())
  {
    m_nThreads = static_cast<int>(m_Items.size());
  }

  m_nItemsPerThread = static_cast<int>(m_Items.size() / m_nThreads);

  m_nIterations = m_nThreads;
  if (m_Items.size() % m_nThreads != 0)
  {
    m_nIterations += 1; // + 1 for remainder items
  }

  m_pTP = new CThreadPool(m_nThreads);
}

template <class TypeInput>
CSTLThreadT<TypeInput>::~CSTLThreadT()
{
  delete m_pTP;
}

template <class TypeInput>
int CSTLThreadT<TypeInput>::Threads() const
{
  return m_nThreads;
}

template <class TypeInput>
int CSTLThreadT<TypeInput>::Iterations() const
{
  return m_nIterations;
}

template <class TypeInput>
const eReturn CSTLThreadT<TypeInput>::Task(
  typename TypeInput::value_type& item, int iteration, int threadid)
{
  assert(NULL && "This method must be overridden");
  return eReturn::Ok;
}

template <class TypeInput>
void CSTLThreadT<TypeInput>::Launch()
{
  this->Initialize();

  for (int iteration = 0; iteration < m_nIterations; iteration++)
  {
    m_pTP->AddTask([=]()
    {
      std::stringstream ss;
      ss << std::this_thread::get_id();
      int threadid = std::stoi(ss.str());

      this->Execute(iteration, threadid);
    });
  }

  m_pTP->Launch();
}

template <class TypeInput>
void CSTLThreadT<TypeInput>::Initialize()
{
  // Override this method to do anything before launching
}

template <class TypeInput>
void CSTLThreadT<TypeInput>::Execute(int iteration, int threadid)
{
  // std::lock_guard<std::mutex> lg(m_Mutex); // TODO: Vic. Recheck. Avoid race condition.

  auto nItems = static_cast<int>(m_Items.size());

  int start = m_nItemsPerThread * iteration;
  int stop  = m_nItemsPerThread * (iteration + 1) - 1; // - 1 because the index is itstarted at 0

  if (stop > nItems - 1)
  {
    stop = nItems - 1;
  }

  auto itstart = std::next(m_Items.begin(), start);
  auto itstop  = std::next(m_Items.begin(), stop + 1); // + 1 to iterate to the last item

  for (auto it = itstart; it != itstop; it++)
  {
    auto item = *it;
    auto ret = Task(item, iteration, threadid);
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