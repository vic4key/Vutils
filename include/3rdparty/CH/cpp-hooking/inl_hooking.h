/**
 * @file   inl_hooking.h
 * @author Vic P.
 * @brief  Header/Implementation for Inline Hooking Manager.
 */

#pragma once

#include "common.h"

#include <vu>

/**
 * @brief Inline Hooking Manager.
 */
class INLHookingManager : public vu::SingletonT<INLHookingManager>
{
  struct INL_Hooked: public Hooked
  {
    vu::INLHooking m_hooker;

    INL_Hooked() : Hooked() {}

    INL_Hooked(const INL_Hooked& right)
    {
      *this = right;
    }

    const INL_Hooked& operator=(const INL_Hooked& right)
    {
      if (this != &right)
      {
        Hooked::operator=(right);
        m_hooker = right.m_hooker;
      }

      return *this;
    }
  };

  std::unordered_map<void*, INL_Hooked> m_list;

public:
  /**
   * @brief Hook a given function.
   * @param[in] function    The function that want to hook.
   * @param[in] hk_function The hooking function.
   * @return true if succeeds otherwise return false.
   */
  template<typename Function>
  bool hook(void* function, Function&& hk_function)
  {
    auto it = m_list.find(function);
    if (it != m_list.cend())
    {
      return false;
    }

    INL_Hooked hooked;
    if (!hooked.m_hooker.attach(function, (void*)hk_function, &hooked.m_trampoline))
    {
      return false;
    }

    using FunctionPtr = decltype(&hk_function);

    hooked.m_function = function;
    hooked.m_invoker.reset(new Invokable(FunctionPtr(hooked.m_trampoline)));

    void* key = hooked.m_function;
    m_list[key] = std::move(hooked);

    return true;
  }

  /**
   * @brief Unhook a given function that was hooked.
   * @param[in] entry The function that want to un-hook.
   * @return true if succeeds otherwise return false.
   */
  bool unhook(void* function)
  {
    auto it = m_list.find(function);
    if (it == m_list.cend())
    {
      return false;
    }

    auto& hooked = it->second;
    auto result = hooked.m_hooker.detach(function, &hooked.m_trampoline);
    if (result)
    {
      m_list.erase(it);
    }

    return result;
  }

  /**
   * @brief Hook a given function.
   * @param[in] module      The module name.
   * @param[in] function    The function name.
   * @param[in] hk_function The hooking function.
   * @return true if succeeds otherwise return false.
   */
  template<typename StdString, typename Function>
  bool hook(const StdString& module, const StdString& function, Function&& hk_function)
  {
    auto ptr = get_proc_address<StdString>(module, function);
    return ptr != nullptr ? this->hook(ptr, hk_function) : false;
  }

  /**
   * @brief Unhook a given function that was hooked.
   * @param[in] module    The module name.
   * @param[in] function  The function name.
   * @return true if succeeds otherwise return false.
   */
  template<typename StdString>
  bool unhook(const StdString& module, const StdString& function)
  {
    auto ptr = get_proc_address<StdString>(module, function);
    return ptr != nullptr ? this->unhook(ptr) : false;
  }

  /**
   * @brief Invoke the original function.
   * @param[in] entry The function that want to invoke.
   * @param[in] args  The arguments that pass to the original function.
   * @return Based on the result of the original function.
   */
  template<typename Return, typename ... Args>
  Return invoke(void* function, Args ... args)
  {
    auto it = m_list.find(function);
    if (it == m_list.cend())
    {
      throw "invoke the function that did not hook";
    }

    auto& hooked = it->second;

    if (std::is_void<Return>::value)
    {
      hooked.m_invoker->invoke<Return>(std::forward<Args>(args)...);
    }
    else
    {
      return hooked.m_invoker->invoke<Return>(std::forward<Args>(args)...);
    }

    return 0;
  }
};
