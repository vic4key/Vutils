/**
 * @file   misc.cpp
 * @author Vic P.
 * @brief  Implementation for Miscellaneous
 */

#include "Vutils.h"
#include "lazy.h"

namespace vu
{

bool vuapi IsAdministrator()
{
  BOOL IsMember = FALSE;
  SID_IDENTIFIER_AUTHORITY IA = SECURITY_NT_AUTHORITY;
  PSID SID;

  if (InitMiscRoutine() != VU_OK)
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

bool SetPrivilegeA(const std::string& Privilege, const bool Enable)
{
  const auto s = ToStringW(Privilege);
  return SetPrivilegeW(s, Enable);
}

bool SetPrivilegeW(const std::wstring& Privilege, const bool Enable)
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
  if (LookupPrivilegeValueW(nullptr, Privilege.c_str(), &luid))
  {
    TOKEN_PRIVILEGES tp = { 0 };
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = Enable ? SE_PRIVILEGE_ENABLED : 0;

    if (AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), nullptr, nullptr))
    {
      result = GetLastError() != ERROR_NOT_ALL_ASSIGNED;
    }
  }

  CloseHandle(hToken);

  return result;
}

std::string vuapi GetEnviromentA(const std::string EnvName)
{
  std::string s;
  s.clear();

  std::unique_ptr<char[]> p(new char [MAX_SIZE]);
  if (p == nullptr)
  {
    return s;
  }

  ZeroMemory(p.get(), MAX_SIZE);

  ulong result = GetEnvironmentVariableA(EnvName.c_str(), p.get(), MAX_SIZE);
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

    result = GetEnvironmentVariableA(EnvName.c_str(), p.get(), result);
    if (result == 0)
    {
      return s;
    }
  }

  s.assign(p.get());

  return s;
}

std::wstring vuapi GetEnviromentW(const std::wstring EnvName)
{
  std::wstring s;
  s.clear();

  std::unique_ptr<wchar[]> p(new wchar [MAX_SIZE]);
  if (p == nullptr)
  {
    return s;
  }

  ZeroMemory(p.get(), 2*MAX_SIZE);

  ulong result = GetEnvironmentVariableW(EnvName.c_str(), p.get(), 2*MAX_SIZE);
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

    result = GetEnvironmentVariableW(EnvName.c_str(), p.get(), 2*result);
    if (result == 0)
    {
      return s;
    }
  }

  s.assign(p.get());

  return s;
}

typedef std::vector<std::pair<bool, byte>> TPattern;

const TPattern ToPattern(const std::string& Buffer)
{
  TPattern result;

  const auto l = vu::SplitStringA(Buffer, " ");
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

std::pair<bool, size_t> FindPatternA(const CBuffer& Buffer, const std::string& Pattern)
{
  std::pair<bool, size_t> result(false, 0);

  if (Buffer.GetpData() == nullptr || Pattern.empty())
  {
    return result;
  }

  const auto Pointer = static_cast<const byte*>(Buffer.GetpData());
  const size_t Size = Buffer.GetSize();

  return FindPatternA(Pointer, Size, Pattern);
}

std::pair<bool, size_t> FindPatternW(const CBuffer& Buffer, const std::wstring& Pattern)
{
  const auto s = ToStringA(Pattern);
  return FindPatternA(Buffer, s);
}

std::pair<bool, size_t> FindPatternA(const void* Pointer, const size_t Size, const std::string& Pattern)
{
  std::pair<bool, size_t> result(false, 0);

  if (Pointer == nullptr || Size == 0 || Pattern.empty())
  {
    return result;
  }

  const auto pattern = ToPattern(Pattern);
  if (pattern.empty())
  {
    return result;
  }

  const auto pointer = static_cast<const byte*>(Pointer);
  const size_t size = Size;

  for (size_t i = 0; i < size; ++i)
  {
    size_t j = 0;

    for (j = 0; j < pattern.size(); ++j)
    {
      if (pattern[j].first && pattern[j].second != pointer[i + j])
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

std::pair<bool, size_t> FindPatternW(const void* Pointer, const size_t Size, const std::wstring& Pattern)
{
  const auto s = ToStringA(Pattern);
  return FindPatternA(Pointer, Size, s);
}

} // namespace vu