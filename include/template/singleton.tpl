/**
 * @file   singleton.tpl
 * @author Vic P.
 * @brief  Template for Singleton Design Pattern
 */

// Singleton

template<typename T>
class CSingletonT
{
public:
  CSingletonT()
  {
    m_pInstance = static_cast<T*>(this);
  }

  virtual ~CSingletonT()
  {
  }

  static T& Instance()
  {
    if (m_pInstance == nullptr)
    {
      CSingletonT<T>::m_pInstance = new T();
    }

    return *m_pInstance;
  }

  static void Destroy()
  {
    if (CSingletonT<T>::m_pInstance != nullptr)
    {
      delete CSingletonT<T>::m_pInstance;
      CSingletonT<T>::m_pInstance = nullptr;
    }
  }

protected:
  static T* m_pInstance;
};

template<typename T>
T* CSingletonT<T>::m_pInstance = nullptr;
