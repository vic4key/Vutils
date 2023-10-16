/**
 * @file   handle.tpl
 * @author Vic P.
 * @brief  Template for Scoped Handle
 */

 /**
  * ScopedHandleT
  */

template<typename traits>
class ScopedHandleT
{
private:
  typename traits::type m_h = traits::invalid;

public:
  ScopedHandleT() : m_h(traits::invalid)
  {
  }

  ScopedHandleT(const typename traits::type h) : m_h(h)
  {
  }

  virtual ~ScopedHandleT()
  {
    this->close();
  }

  const ScopedHandleT& operator=(const ScopedHandleT& right)
  {
    m_h = right.m_h;
    return *this;
  }

  const ScopedHandleT& operator=(const typename traits::type h)
  {
    m_h = h;
    return *this;
  }

  operator bool() const
  {
    return m_h != traits::invalid;
  }

  bool operator!() const
  {
    return !bool(*this);
  }

  operator typename traits::type() const
  {
    return m_h;
  }

  void close()
  {
    if (m_h != traits::invalid)
    {
      traits::close(m_h);
      m_h = traits::invalid;
    }
  }
};

#define ScopedHandleT_Define(object_name, data_type, invalid_value, oneliner_codes)\
struct _deleter_ ## object_name\
{\
  typedef data_type type;\
  static constexpr type invalid = invalid_value;\
  static void close(type h) { oneliner_codes; }\
};\
using Scoped_ ## object_name = ScopedHandleT<_deleter_ ## object_name>;
