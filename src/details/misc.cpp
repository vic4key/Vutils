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

bool SetPrivilegeA(const std::string& privilege, const bool enable)
{
  const auto s = ToStringW(privilege);
  return SetPrivilegeW(s, enable);
}

bool SetPrivilegeW(const std::wstring& privilege, const bool enable)
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

} // namespace vu