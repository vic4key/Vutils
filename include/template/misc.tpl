/**
 * @file   misc.tpl
 * @author Vic P.
 * @brief  Template for Miscellaneous
 */

// Misc

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || (__cplusplus >= 201703L))

#include <iostream>

void print()
{
  std::cout << std::endl;
}

template <typename Head, typename ...Tail>
void print(Head&& head, Tail&&... tail)
{
  std::cout << head;
  if (sizeof...(tail) != 0) std::cout << " ";
  print(tail...);
}

void wprint()
{
  std::cout << std::endl;
}

template <typename Head, typename ...Tail>
void wprint(Head&& head, Tail&&... tail)
{
  std::wcout << head;
  if (sizeof...(tail) != 0) std::wcout << L" ";
  wprint(tail...);
}

#endif
