/**
 * @file   invokable.h
 * @author Vic P.
 * @brief  Header/Implementation for the function-container that can hold any function prototype.
 */

#pragma once

#ifndef _ANY_
#include <any>
#include <functional>
#endif  // _ANY_

/**
 * @brief The function-container that can hold any function prototype.
 */
struct Invokable
{
  Invokable() {}

  template<typename Function>
  Invokable(Function&& function) : Invokable(std::function(std::forward<Function>(function))) {}

  template<typename Return, typename ... Args>
  Invokable(std::function<Return(Args...)> function) : m_function(function)
  {
    m_function = function;
  }

  template<typename Return, typename ... Args>
  Return invoke(Args ... args)
  {
    auto function = std::any_cast<std::function<Return(Args...)>>(m_function);

    if (std::is_void<Return>::value)
    {
      std::invoke(function, std::forward<Args>(args)...);
    }
    else
    {
      return std::invoke(function, std::forward<Args>(args)...);
    }
  }

  std::any m_function;
};
