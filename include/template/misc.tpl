/**
 * @file   misc.tpl
 * @author Vic P.
 * @brief  Template for Miscellaneous
 */

// Misc

// C++14 (MSVC 2015+ or MinGW 5.1+)
#if (defined(_MSC_VER) && _MSC_VER >= 1900) || (defined(__MINGW32__) && __cplusplus >= 201402L)

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

#define VU_HAS_PRINT

#endif
