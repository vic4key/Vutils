/**
 * @file   common.h
 * @author Vic P.
 * @brief  Header for common use.
 */

#pragma once

#include "invokable.h"

#include <memory>
#include <vu>

/**
 * @brief The structure that holds the hooking information of a function.
 */
struct Hooked
{
  void* m_function;
  void* m_trampoline;
  std::shared_ptr<Invokable> m_invoker;

  Hooked() : m_function(nullptr), m_trampoline(nullptr) {}

  Hooked(const Hooked& right)
  {
    *this = right;
  }

  const Hooked& operator=(const Hooked& right)
  {
    if (this != &right)
    {
      m_function   = right.m_function;
      m_trampoline = right.m_trampoline;
      m_invoker    = right.m_invoker;
    }

    return *this;
  }
};

template <typename T>
struct LibraryT;

template <>
struct LibraryT<std::string>
{
  typedef vu::LibraryA self;
};

template <>
struct LibraryT<std::wstring>
{
  typedef vu::LibraryW self;
};

/**
 * @brief Get the address of an exported function from the given dynamic-link module.
 * @param   module    The module name.
 * @param   function  The function name.
 * @returns The address of the function or null.
 */
template<typename StdString>
inline void* get_proc_address(const StdString& module, const StdString& function)
{
  if (module.empty() || function.empty())
  {
    return nullptr;
  }

  return LibraryT<StdString>::self::quick_get_proc_address(module, function);
}
