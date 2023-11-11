/**
 * @file   iat_hooking.h
 * @author Vic P.
 * @brief  Header/Implementation for IAT Hooking Manager.
 */

#pragma once

#include "common.h"

#include <vu>

/**
 * @brief IAT Hooking Manager.
 */
class IATHookingManager : public vu::SingletonT<IATHookingManager>
{
  struct IAT_Hooked : public Hooked {};

  std::unordered_map<void*, IAT_Hooked> m_list;

  using Entry = vu::IATHookingA::Entry;

  vu::IATHookingA& hooker()
  {
    return vu::IATHookingA::instance();
  }

  void* get_proc_address(const Entry& entry)
  {
    Entry temp;

    if (!this->hooker().exist(entry.target, entry.module, entry.function, &temp))
    {
      return nullptr;
    }

    return temp.original;
  }

public:
  /**
   * @brief Hook a given function.
   * @param[in] entry       The entry of function that want to hook ({ tartget name, module name, function name }).
   * @param[in] hk_function The hooking function.
   * @return true if succeeds otherwise return false.
   */
  template<typename Function>
  bool hook(const Entry& entry, Function&& hk_function)
  {
    IAT_Hooked hooked;

    auto ret = this->hooker().install(entry.target, entry.module, entry.function,
      (void*)hk_function, &hooked.m_trampoline);
    if (ret != vu::VU_OK)
    {
      return false;
    }

    auto function = this->get_proc_address(entry);
    if (function == nullptr)
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
   * @param[in] entry The entry of function that want to un-hook ({ tartget name, module name, function name }).
   * @return true if succeeds otherwise return false.
   */
  bool unhook(const Entry& entry)
  {
    auto function = this->get_proc_address(entry);
    if (function == nullptr)
    {
      return false;
    }

    auto it = m_list.find(function);
    if (it == m_list.cend())
    {
      return false;
    }

    auto ret = this->hooker().uninstall(entry.target, entry.module, entry.function);
    if (ret != vu::VU_OK)
    {
      return false;
    }

    m_list.erase(it);

    return true;
  }

  /**
   * @brief Invoke the original function.
   * @param[in] entry The entry of function that want to invoke ({ tartget name, module name, function name }).
   * @param[in] args  The arguments that pass to the original function.
   * @return Based on the result of the original function.
   */
  template<typename Return, typename ... Args>
  Return invoke(const Entry& entry, Args ... args)
  {
    auto function = this->get_proc_address(entry);
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
  }
};
