#include <iostream>
#include <sstream>
#include <tuple>

#ifdef EASY_PRINT_WINDOWS

#include <windows.h>

namespace easyprint_helper // @refer to https://github.com/vic4key/Vutils.git
{
  inline std::string __to_string_A(const std::wstring& string, const bool utf8)
  {
    std::string s;
    s.clear();

    if (string.empty())
    {
      return s;
    }

    int N = (int)string.length();

    if (utf8)
    {
      N = WideCharToMultiByte(CP_UTF8, 0, string.c_str(), int(string.length()), NULL, 0, NULL, NULL);
    }

    N += 1;

    std::unique_ptr<char[]> p(new char[N]);
    if (p == nullptr)
    {
      return s;
    }

    ZeroMemory(p.get(), N);

    WideCharToMultiByte(utf8 ? CP_UTF8 : CP_ACP, WC_COMPOSITECHECK, string.c_str(), -1, p.get(), N, NULL, NULL);

    s.assign(p.get());

    return s;
  }

  inline std::wstring __to_string_W(const std::string& string, const bool utf8)
  {
    std::wstring s;
    s.clear();

    if (string.empty())
    {
      return s;
    }

    int N = (int)string.length();

    if (utf8)
    {
      N = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), int(string.length()), NULL, 0);
    }

    N += 1;

    std::unique_ptr<wchar_t[]> p(new wchar_t[N]);
    if (p == nullptr)
    {
      s.clear();
    }

    ZeroMemory(p.get(), 2 * N);

    MultiByteToWideChar(utf8 ? CP_UTF8 : CP_ACP, 0, string.c_str(), -1, p.get(), 2 * N);

    s.assign(p.get());

    return s;
  }
} // easyprint_helper

#endif // EASY_PRINT_WINDOWS

namespace easyprint {

template <typename ...Targs>
struct default_delimiter {
  static constexpr const char* start_delimiter = "{";
  static constexpr const char* element_delimiter = ", ";
  static constexpr const char* end_delimiter = "}";
};

template <size_t I> struct tuple_delimiter {
  static constexpr const char* value = default_delimiter<int>::element_delimiter;
};

template <> struct tuple_delimiter<0> { static constexpr const char* value = default_delimiter<int>::start_delimiter; };

template <typename T> struct is_tuple { static const bool value = false; };

template <typename... Targs> struct is_tuple<std::tuple<Targs...>> {
  static const bool value = true;
};

// Helper struct to determine
// whether a type has a const_iterator typedef
template <typename T> struct is_const_iterable {

  template <typename C> static std::true_type f(typename C::const_iterator *);

  template <typename C> static std::false_type f(...);

  typedef decltype(f<T>(0)) type;
};

template <typename T>
using is_const_iterable_v = typename is_const_iterable<T>::type;

template <typename T>
inline void print_iterator_helper(std::false_type, std::ostream &os, const T &cont) {
  os << cont;
}

inline void print_iterator_helper(std::false_type, std::ostream &os, const char *cont) {
  os << "\"" << cont << "\"";
}

template <typename TChar>
inline void print_iterator_helper(std::true_type, std::ostream &os,
                           const std::basic_string<TChar> &cont) {
  os << "\"" << cont << "\"";
}

#ifdef EASY_PRINT_WINDOWS
template <>
inline void print_iterator_helper(std::true_type, std::ostream& os,
  const std::basic_string<wchar_t>& cont) {
  os << "\"" << easyprint_helper::__to_string_A(cont, true) << "\"";
}
#endif // EASY_PRINT_WINDOWS

// Functions to recursively print tuples
template <size_t I, typename T>
typename std::enable_if<(!is_tuple<T>::value), void>::type
print_tuple(std::ostream &os, const T &cont) {
  print_iterator_helper(is_const_iterable_v<T>(), os, cont);
}

template <size_t I, typename... Targs>
typename std::enable_if<(I == sizeof...(Targs)), void>::type
print_tuple(std::ostream &os, const std::tuple<Targs...> &tup) {
  os << default_delimiter<std::tuple<Targs...>>::end_delimiter;
}

template <size_t I, typename... Targs>
typename std::enable_if<(I < sizeof...(Targs)), void>::type
print_tuple(std::ostream &os, const std::tuple<Targs...> &tup) {
  os << tuple_delimiter<I>::value;
  auto val = std::get<I>(tup);
  print_tuple<0>(os, val);
  print_tuple<I + 1>(os, tup);
}

template <typename T>
inline void print_iterator_helper(std::true_type, std::ostream &os, const T &cont);

// Pair specialization
template <typename T1, typename T2>
inline void print_iterator_helper(std::false_type, std::ostream &os,
                           const std::pair<T1, T2> &cont) {
  os << default_delimiter<decltype(cont)>::start_delimiter;
  print_iterator_helper(is_const_iterable_v<T1>(), os, cont.first);
  os << default_delimiter<decltype(cont)>::element_delimiter;
  print_iterator_helper(is_const_iterable_v<T2>(), os, cont.second);
  os << default_delimiter<decltype(cont)>::end_delimiter;
}

// Specialisation for tuples
// Passes control to tuple printing
// functions
template <typename... Targs>
inline void print_iterator_helper(std::false_type, std::ostream &os,
                           const std::tuple<Targs...> &cont) {
  print_tuple<0>(os, cont);
}

// Recursive function to print iterators
template <typename T>
inline void print_iterator_helper(std::true_type, std::ostream &os, const T &cont) {
  os << default_delimiter<decltype(cont)>::start_delimiter;
  if (!cont.empty()) {
    auto it = cont.begin();
    print_iterator_helper(is_const_iterable_v<typename T::value_type>{}, os, *it);
    it++;
    for (; it != cont.cend(); it++) {
      os << default_delimiter<decltype(cont)>::element_delimiter;
      print_iterator_helper(is_const_iterable_v<typename T::value_type>{}, os, *it);
    }
  }
  os << default_delimiter<decltype(cont)>::end_delimiter;
}

// User-facing functions
template <typename T>
inline std::string stringify_A(const T &container) {
  std::stringstream ss;
  print_iterator_helper(is_const_iterable_v<T>(), ss, container);
  return ss.str();
}

template <typename T>
inline std::wstring stringify_W(const T& container) {
  std::stringstream ss;
  print_iterator_helper(is_const_iterable_v<T>(), ss, container);
  return easyprint_helper::__to_string_W(ss.str(), true);
}

template <typename T>
inline void print_A(const T &container) {
  std::cout << stringify_A(container);
}

template <typename T>
inline void print_W(const T& container) {
  std::wcout << stringify_W(container);
}

#ifdef _UNICODE
#define stringify stringify_W
#define print print_W
#else // _UNICODE
#define stringify stringify_A
#define print print_A
#endif // _UNICODE

}  // namespace easyprint
