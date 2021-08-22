/**
 * @file   misc.cpp
 * @author Vic P.
 * @brief  Implementation for Miscellaneous
 */

#include "Vutils.h"
#include "lazy.h"
#include "defs.h"

#include VU_3RD_INCL(UND/include/undname.h)

namespace vu
{

bool vuapi is_administrator()
{
  BOOL IsMember = FALSE;
  SID_IDENTIFIER_AUTHORITY IA = SECURITY_NT_AUTHORITY;
  PSID SID;

  if (Initialize_DLL_MISC() != VU_OK)
  {
    return FALSE;
  }

  if (AllocateAndInitializeSid(&IA, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &SID))
  {
    if (!pfnCheckTokenMembership(NULL, SID, &IsMember))
    {
      IsMember = FALSE;
    }

    FreeSid(SID);
  }

  return (IsMember != FALSE);
}

bool set_privilege_A(const std::string& privilege, const bool enable)
{
  const auto s = to_string_W(privilege);
  return set_privilege_W(s, enable);
}

bool set_privilege_W(const std::wstring& privilege, const bool enable)
{
  HANDLE hToken = INVALID_HANDLE_VALUE;
  if (!OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken))
  {
    if (GetLastError() != ERROR_NO_TOKEN)
    {
      return false;
    }

    if (!ImpersonateSelf(SecurityImpersonation))
    {
      return false;
    }

    if (!OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken))
    {
      return false;
    }
  }

  if (hToken == INVALID_HANDLE_VALUE)
  {
    return false;
  }

  bool result = false;

  LUID luid = { 0 };
  if (LookupPrivilegeValueW(nullptr, privilege.c_str(), &luid))
  {
    TOKEN_PRIVILEGES tp = { 0 };
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = enable ? SE_PRIVILEGE_ENABLED : 0;

    if (AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), nullptr, nullptr))
    {
      result = GetLastError() != ERROR_NOT_ALL_ASSIGNED;
    }
  }

  CloseHandle(hToken);

  return result;
}

std::string vuapi get_enviroment_A(const std::string name)
{
  std::string s;
  s.clear();

  std::unique_ptr<char[]> p(new char [MAX_SIZE]);
  if (p == nullptr)
  {
    return s;
  }

  ZeroMemory(p.get(), MAX_SIZE);

  ulong result = GetEnvironmentVariableA(name.c_str(), p.get(), MAX_SIZE);
  if (result == ERROR_ENVVAR_NOT_FOUND || result == 0)
  {
    return s;
  }
  else if (result > MAX_SIZE)
  {
    result += 1;
    p.reset(new char[result]);
    if (p == nullptr)
    {
      return s;
    }

    ZeroMemory(p.get(), result);

    result = GetEnvironmentVariableA(name.c_str(), p.get(), result);
    if (result == 0)
    {
      return s;
    }
  }

  s.assign(p.get());

  return s;
}

std::wstring vuapi get_enviroment_W(const std::wstring name)
{
  std::wstring s;
  s.clear();

  std::unique_ptr<wchar[]> p(new wchar [MAX_SIZE]);
  if (p == nullptr)
  {
    return s;
  }

  ZeroMemory(p.get(), 2*MAX_SIZE);

  ulong result = GetEnvironmentVariableW(name.c_str(), p.get(), 2*MAX_SIZE);
  if (result == ERROR_ENVVAR_NOT_FOUND || result == 0)
  {
    return s;
  }
  else if (result > MAX_SIZE)
  {
    result += 1;
    p.reset(new wchar[result]);
    if (p == nullptr)
    {
      return s;
    }

    ZeroMemory(p.get(), 2*result);

    result = GetEnvironmentVariableW(name.c_str(), p.get(), 2*result);
    if (result == 0)
    {
      return s;
    }
  }

  s.assign(p.get());

  return s;
}

typedef std::vector<std::pair<bool, byte>> TPattern;

const TPattern to_pattern(const std::string& buffer)
{
  TPattern result;

  const auto l = vu::split_string_A(buffer, " ");
  for (const auto& e : l)
  {
    auto v = TPattern::value_type(false, 0x00);

    if (e.length() == 2 && isxdigit(e[0]) && isxdigit(e[1]))
    {
      v.first  = true;
      v.second = (byte)strtoul(e.c_str(), nullptr, 16);
    }

    result.emplace_back(v);
  }

  return result;
}

std::pair<bool, size_t> find_pattern_A(const Buffer& buffer, const std::string& pattern)
{
  std::pair<bool, size_t> result(false, 0);

  if (buffer.get_ptr_data() == nullptr || pattern.empty())
  {
    return result;
  }

  const auto pointer = static_cast<const byte*>(buffer.get_ptr_data());
  const size_t size = buffer.get_size();

  return find_pattern_A(pointer, size, pattern);
}

std::pair<bool, size_t> find_pattern_W(const Buffer& buffer, const std::wstring& pattern)
{
  const auto s = to_string_A(pattern);
  return find_pattern_A(buffer, s);
}

std::pair<bool, size_t> find_pattern_A(const void* ptr, const size_t size, const std::string& pattern)
{
  std::pair<bool, size_t> result(false, 0);

  if (ptr == nullptr || size == 0 || pattern.empty())
  {
    return result;
  }

  const auto patternn = to_pattern(pattern);
  if (pattern.empty())
  {
    return result;
  }

  const auto pointer = static_cast<const byte*>(ptr);

  for (size_t i = 0; i < size; ++i)
  {
    size_t j = 0;

    for (j = 0; j < pattern.size(); ++j)
    {
      if (patternn[j].first && patternn[j].second != pointer[i + j])
      {
        break;
      }
    }

    if (j == pattern.size())
    {
      result = std::make_pair(true, i);
      break;
    }
  }

  return result;
}

std::pair<bool, size_t> find_pattern_W(const void* ptr, const size_t size, const std::wstring& pattern)
{
  const auto s = to_string_A(pattern);
  return find_pattern_A(ptr, size, s);
}

std::string undecorate_cpp_symbol_A(const std::string& name, const ushort flags)
{
  char s[KB] = { 0 };
  memset(&s, 0, sizeof(s));
  undname(reinterpret_cast<char*>(&s), const_cast<char*>(name.c_str()), sizeof(s), flags);
  return std::string(s);
}

std::wstring undecorate_cpp_symbol_W(const std::wstring& name, const ushort flags)
{
  auto s = to_string_A(name);
  auto r = undecorate_cpp_symbol_A(s, flags);
  return to_string_W(r);
}

} // namespace vu