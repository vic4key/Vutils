/**
 * @file   singleton.tpl
 * @author Vic P.
 * @brief  Template for Singleton Design Pattern
 */

 /**
  * CSingletonT
  */

template<typename T>
class CSingletonT
{
public:
  CSingletonT()
  {
    m_ptr_instance = static_cast<T*>(this);
  }

  virtual ~CSingletonT()
  {
  }

  static T& instance()
  {
    if (m_ptr_instance == nullptr)
    {
      CSingletonT<T>::m_ptr_instance = new T();
    }

    return *m_ptr_instance;
  }

  static void destroy()
  {
    if (CSingletonT<T>::m_ptr_instance != nullptr)
    {
      delete CSingletonT<T>::m_ptr_instance;
      CSingletonT<T>::m_ptr_instance = nullptr;
    }
  }

protected:
  static T* m_ptr_instance;
};

template<typename T>
T* CSingletonT<T>::m_ptr_instance = nullptr;
