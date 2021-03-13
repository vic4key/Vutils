/**
 * @file   stlthread.tpl
 * @author Vic P.
 * @brief  Template for STL Threading
 */

 /**
  * CSTLThreadT
  */

#define TypeNone
#define CSTLThreadTArgTypes(type) type TypeItem, type TypeInput, type TypeResult

enum eReturn
{
  Ok,
  Break,
  Continue,
};

template <CSTLThreadTArgTypes(typename)>
class CSTLThreadT
{
public:
  CSTLThreadT(const TypeInput& items, void* pData = nullptr, int nThreads = MAX_NTHREADS);
  virtual ~CSTLThreadT();
  virtual void Initialize();
  virtual void Execute(int iteration, int threadID);
  virtual const eReturn Task(TypeItem& item, void* pdata, int iteration, int threadid);
  virtual const TypeResult& Result() const;
  virtual void  Launch();

protected:
  CThreadPool* m_pTP;
  std::mutex m_Mutex;

  int m_nThreads;
  int m_nItemsPerThread;

  void* m_pData;
  TypeResult m_Result;
  const TypeInput& m_Items;
};

template <CSTLThreadTArgTypes(typename)>
CSTLThreadT<CSTLThreadTArgTypes(TypeNone)>::CSTLThreadT(
  const TypeInput& items, void* pData, int nThreads)
  : m_Items(items), m_pData(pData) , m_nThreads(nThreads)
{
  if (m_nThreads == MAX_NTHREADS)
  {
    m_nThreads = std::thread::hardware_concurrency();
  }

  m_pTP = new CThreadPool(m_nThreads);

  if (m_nThreads > m_Items.size())
  {
    m_nThreads = static_cast<int>(m_Items.size());
  }

  m_nItemsPerThread = static_cast<int>(m_Items.size() / m_nThreads);

  if (std::is_fundamental<TypeResult>::value)
  {
    memset(&m_Result, 0, sizeof(m_Result));
  }
}

template <CSTLThreadTArgTypes(typename)>
CSTLThreadT<CSTLThreadTArgTypes(TypeNone)>::~CSTLThreadT()
{
  delete m_pTP;
}

template <CSTLThreadTArgTypes(typename)>
const TypeResult& CSTLThreadT<CSTLThreadTArgTypes(TypeNone)>::Result() const
{
  return m_Result;
}

template <CSTLThreadTArgTypes(typename)>
const eReturn CSTLThreadT<CSTLThreadTArgTypes(TypeNone)>::Task(
  TypeItem& item, void* pdata, int iteration, int threadid)
{
  assert(0 && "This method must be overridden");
  return eReturn::Ok;
}

template <CSTLThreadTArgTypes(typename)>
void CSTLThreadT<CSTLThreadTArgTypes(TypeNone)>::Launch()
{
  this->Initialize();
  m_pTP->Launch();
}

template <CSTLThreadTArgTypes(typename)>
void CSTLThreadT<CSTLThreadTArgTypes(TypeNone)>::Initialize()
{
  int nIterations = m_nThreads;
  if (m_Items.size() % m_nThreads != 0)
  {
    nIterations += 1;
  }

  for (int iteration = 0; iteration < nIterations; iteration++)
  {
    m_pTP->AddTask([=]()
    {
      std::stringstream ss;
      ss << std::this_thread::get_id();
      int threadid = std::stoi(ss.str());

      this->Execute(iteration, threadid);
    });
  }
}

template <CSTLThreadTArgTypes(typename)>
void CSTLThreadT<CSTLThreadTArgTypes(TypeNone)>::Execute(int iteration, int threadid)
{
  // std::lock_guard<std::mutex> lg(m_Mutex); // TODO: Vic. Recheck. Avoid race condition m_Result.

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
    auto ret = Task(item, m_pData, iteration, threadid);
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