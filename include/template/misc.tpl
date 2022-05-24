/**
 * @file   misc.tpl
 * @author Vic P.
 * @brief  Template for Miscellaneous
 */

// Misc

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || (__cplusplus >= 201703L))

#include <iostream>

static void print_A()
{
  std::cout << std::endl;
}

template <typename Head, typename ...Tail>
void print_A(Head&& head, Tail&&... tail)
{
  std::cout << head;
  if (sizeof...(tail) != 0) std::cout << " ";
  print_A(tail...);
}

static void print_W()
{
  std::wcout << std::endl;
}

template <typename Head, typename ...Tail>
void print_W(Head&& head, Tail&&... tail)
{
  std::wcout << head;
  if (sizeof...(tail) != 0) std::wcout << L" ";
  print_W(tail...);
}

#ifdef _UNICODE
#define print print_W
#else
#define print print_A
#endif

#endif
