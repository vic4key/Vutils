/**
 * @file   singleton.tpl
 * @author Vic P.
 * @brief  Template for Singleton Design Pattern
 */

 /**
  * SingletonT
  */

template<typename T>
class SingletonT
{
public:
  SingletonT()
  {
    m_ptr_instance = static_cast<T*>(this);
  }

  virtual ~SingletonT()
  {
  }

  static T& instance()
  {
    if (m_ptr_instance == nullptr)
    {
      SingletonT<T>::m_ptr_instance = new T();
    }

    return *m_ptr_instance;
  }

  static void destroy()
  {
    if (SingletonT<T>::m_ptr_instance != nullptr)
    {
      delete SingletonT<T>::m_ptr_instance;
      SingletonT<T>::m_ptr_instance = nullptr;
    }
  }

protected:
  static T* m_ptr_instance;
};

template<typename T>
T* SingletonT<T>::m_ptr_instance = nullptr;
