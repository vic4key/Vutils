/**
 * @file   misc.tpl
 * @author Vic P.
 * @brief  Template for Miscellaneous
 */

// Misc

// C++14 (MSVC 2015+ or MinGW 5.1+)
#if (defined(_MSC_VER) && _MSVC_LANG >= 201402L) || (defined(__MINGW32__) && __cplusplus >= 201402L)

#include <sstream>
#include <iostream>

/**
 * print(...) (Python like)
 */

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

/**
 * _cout(....) // push all items into string-stream then use `std::cout` out to STDOUT with-in a single out
 */

template<typename Stream>
void __vu_cout_impl(Stream& stream) {}

template<typename Stream, typename T, typename... Args>
void __vu_cout_impl(Stream& stream, T&& t, Args&&... args)
{
  stream << std::forward<T>(t);
  __vu_cout_impl(stream, std::forward<Args>(args)...);
}

template<typename... Args>
void _cout_A(Args&&... args)
{
  std::stringstream ss;
  __vu_cout_impl(ss, std::forward<Args>(args)...);
  std::cout << ss.str();
}

template<typename... Args>
void _cout_W(Args&&... args)
{
  std::wstringstream ss;
  __vu_cout_impl(ss, std::forward<Args>(args)...);
  std::wcout << ss.str();
}

#ifdef _UNICODE
#define _cout _cout_W
#else
#define _cout _cout_A
#endif

#define VU_HAS_PRINT

#endif
