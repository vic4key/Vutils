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
    if (SingletonT<T>::m_ptr_instance != nullptr)
    {
      delete SingletonT<T>::m_ptr_instance;
      SingletonT<T>::m_ptr_instance = nullptr;
    }
  }

  SingletonT(SingletonT&&) = delete;
  SingletonT(SingletonT const&) = delete;
  SingletonT& operator=(SingletonT&&) = delete;
  SingletonT& operator=(SingletonT const&) = delete;

  static T& instance()
  {
    if (m_ptr_instance == nullptr)
    {
      SingletonT<T>::m_ptr_instance = new T();
    }

    return *m_ptr_instance;
  }

protected:
  static T* m_ptr_instance;
};

template<typename T>
T* SingletonT<T>::m_ptr_instance = nullptr;
