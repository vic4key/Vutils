/**
 * @file   misc.cpp
 * @author Vic P.
 * @brief  Implementation for Miscellaneous
 */

#include "Vutils.h"
#include "lazy.h"
#include "defs.h"

#if defined(_MSC_VER) || defined(__BCPLUSPLUS__) // LNK
#include "shobjidl.h"
#include "objbase.h"
#include "shlguid.h"
#endif // LNK

#include VU_3RD_INCL(UND/include/undname.h)

namespace vu
{

bool vuapi is_administrator()
{
  BOOL is_member = FALSE;
  SID_IDENTIFIER_AUTHORITY ia = SECURITY_NT_AUTHORITY;
  PSID sid;

  if (Initialize_DLL_MISC() != VU_OK)
  {
    return FALSE;
  }

  if (AllocateAndInitializeSid(
    &ia, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &sid))
  {
    if (!pfnCheckTokenMembership(NULL, sid, &is_member))
    {
      is_member = FALSE;
    }

    FreeSid(sid);
  }

  return (is_member != FALSE);
}

bool set_privilege_A(const std::string& privilege, const bool enable)
{
  const auto s = to_string_W(privilege);
  return set_privilege_W(s, enable);
}

bool set_privilege_W(const std::wstring& privilege, const bool enable)
{
  HANDLE htoken = INVALID_HANDLE_VALUE;
  if (!OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &htoken))
  {
    if (GetLastError() != ERROR_NO_TOKEN)
    {
      return false;
    }

    if (!ImpersonateSelf(SecurityImpersonation))
    {
      return false;
    }

    if (!OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &htoken))
    {
      return false;
    }
  }

  if (htoken == INVALID_HANDLE_VALUE)
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

    if (AdjustTokenPrivileges(htoken, FALSE, &tp, sizeof(tp), nullptr, nullptr))
    {
      result = GetLastError() != ERROR_NOT_ALL_ASSIGNED;
    }
  }

  CloseHandle(htoken);

  return result;
}

std::string vuapi get_env_A(const std::string& name)
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

std::wstring vuapi get_env_W(const std::wstring& name)
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

bool vuapi set_env_A(const std::string& name, const std::string& value)
{
  return SetEnvironmentVariableA(name.c_str(), value.c_str()) != FALSE;
}

bool vuapi set_env_W(const std::wstring& name, const std::wstring& value)
{
  return SetEnvironmentVariableW(name.c_str(), value.c_str()) != FALSE;
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

std::vector<size_t> find_pattern_A(
  const Buffer& buffer, const std::string& pattern, const bool first_match_only)
{
  std::vector<size_t> result;

  if (buffer.get_ptr() == nullptr || pattern.empty())
  {
    return result;
  }

  const auto pointer = static_cast<const byte*>(buffer.get_ptr());
  const size_t size = buffer.get_size();

  return find_pattern_A(pointer, size, pattern, first_match_only);
}

std::vector<size_t> find_pattern_W(
  const Buffer& buffer, const std::wstring& pattern, const bool first_match_only)
{
  const auto s = to_string_A(pattern);
  return find_pattern_A(buffer, s, first_match_only);
}

std::vector<size_t> find_pattern_A(
  const void* ptr, const size_t size, const std::string& pattern, const bool first_match_only)
{
  std::vector<size_t> result;

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

  for (size_t i = 0; i < size; i++)
  {
    size_t j = 0;

    for (j = 0; j < patternn.size(); j++)
    {
      if (patternn[j].first && patternn[j].second != pointer[i + j])
      {
        break;
      }
    }

    if (j == patternn.size())
    {
      result.push_back(i);

      if (first_match_only)
      {
        break;
      }
    }
  }

  return result;
}

std::vector<size_t> find_pattern_W(
  const void* ptr, const size_t size, const std::wstring& pattern, const bool first_match_only)
{
  const auto s = to_string_A(pattern);
  return find_pattern_A(ptr, size, s, first_match_only);
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

/**
 * refer to
 *  https://docs.microsoft.com/en-us/windows/win32/shell/links#creating-a-shortcut-and-a-folder-shortcut-to-a-file
 *  https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getlongpathnamea#remarks
 */

#if defined(_MSC_VER) || defined(__BCPLUSPLUS__) // LNK

std::unique_ptr<LNKA> parse_shortcut_lnk_A(HWND hwnd, const std::string& lnk_file_path)
{
  std::unique_ptr<LNKA> result(nullptr);

  auto ptr = parse_shortcut_lnk_W(hwnd, to_string_W(lnk_file_path));
  if (ptr != nullptr)
  {
    result.reset(new LNKA);
    result->path = to_string_A(ptr->path);
    result->argument = to_string_A(ptr->argument);
    result->directory = to_string_A(ptr->directory);
    result->description = to_string_A(ptr->description);
    result->hotkey = ptr->hotkey;
    result->window = ptr->window;
  }

  return result;
}

std::unique_ptr<LNKW> parse_shortcut_lnk_W(HWND hwnd, const std::wstring& lnk_file_path)
{
  std::unique_ptr<LNKW> result(nullptr);

  CoInitialize(nullptr);

  IShellLinkW* psl = nullptr;
  HRESULT hres = CoCreateInstance(
    CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
  if (SUCCEEDED(hres))
  {
    IPersistFile* ppf = nullptr;
    hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
    if (SUCCEEDED(hres))
    {
      hres = ppf->Load(lnk_file_path.c_str(), STGM_READ);
      if (SUCCEEDED(hres))
      {
        hres = psl->Resolve(hwnd, 0);
        if (SUCCEEDED(hres))
        {
          result.reset(new LNKW);

          WIN32_FIND_DATAW wfd = { 0 };
          WCHAR buffer[KiB] = { 0 }, tmp[KiB] = { 0 };

          memset(buffer, 0, sizeof(buffer));
          hres = psl->GetPath(buffer, ARRAYSIZE(buffer), &wfd, SLGP_SHORTPATH);
          if (SUCCEEDED(hres))
          {
            if (contains_string_W(buffer, L"~"))
            {
              memset(tmp, 0, sizeof(tmp));
              GetLongPathNameW(buffer, tmp, ARRAYSIZE(tmp));
              result->path.assign(tmp);
            }
            else
            {
              result->path.assign(buffer);
            }
          }

          memset(buffer, 0, sizeof(buffer));
          hres = psl->GetArguments(buffer, ARRAYSIZE(buffer));
          if (SUCCEEDED(hres))
          {
            result->argument.assign(buffer);
          }

          memset(buffer, 0, sizeof(buffer));
          hres = psl->GetWorkingDirectory(buffer, ARRAYSIZE(buffer));
          if (SUCCEEDED(hres))
          {
            if (contains_string_W(buffer, L"~"))
            {
              memset(tmp, 0, sizeof(tmp));
              GetLongPathNameW(buffer, tmp, ARRAYSIZE(tmp));
              result->directory.assign(tmp);
            }
            else
            {
              result->directory.assign(buffer);
            }
            if (result->directory.empty())
            {
              result->directory = extract_file_directory_W(result->path);
            }
          }

          memset(buffer, 0, sizeof(buffer));
          hres = psl->GetDescription(buffer, ARRAYSIZE(buffer));
          if (SUCCEEDED(hres))
          {
            result->description.assign(buffer);
          }

          WORD hotkey = 0;
          hres = psl->GetHotkey(&hotkey);
          if (SUCCEEDED(hres))
          {
            result->hotkey = hotkey;
          }

          int icon_index = 0;
          memset(buffer, 0, sizeof(buffer));
          psl->GetIconLocation(buffer, ARRAYSIZE(buffer), &icon_index);
          if (SUCCEEDED(hres))
          {
            result->icon.first.assign(buffer);
            result->icon.second = icon_index;
          }

          int window = SW_NORMAL;
          hres = psl->GetShowCmd(&window);
          if (SUCCEEDED(hres))
          {
            result->window = window;
          }
        }
      }

      ppf->Release();
    }

    psl->Release();
  }

  CoUninitialize();

  return result;
}

vu::VUResult create_shortcut_lnk_A(const std::string& lnk_file_path, const LNKA& lnk)
{
  CoInitialize(nullptr);

  IShellLinkA* psl = nullptr;
  HRESULT hres = CoCreateInstance(
    CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkA, (LPVOID*)&psl);
  if (SUCCEEDED(hres))
  {
    psl->SetPath(lnk.path.c_str());
    psl->SetArguments(lnk.argument.c_str());
    psl->SetWorkingDirectory(lnk.directory.c_str());
    psl->SetHotkey(lnk.hotkey);
    psl->SetShowCmd(lnk.window);
    psl->SetDescription(lnk.description.c_str());
    psl->SetIconLocation(lnk.icon.first.c_str(), lnk.icon.second);

    IPersistFile* ppf = nullptr;
    hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
    if (SUCCEEDED(hres))
    {
      const auto lnk_file_path_tmp = to_string_W(lnk_file_path);
      ppf->Save(lnk_file_path_tmp.c_str(), TRUE);
      ppf->Release();
    }

    psl->Release();
  }

  CoUninitialize();

  return VU_OK;
}

vu::VUResult create_shortcut_lnk_W(const std::wstring& lnk_file_path, const LNKW& lnk)
{
  CoInitialize(nullptr);

  IShellLinkW* psl = nullptr;
  HRESULT hres = CoCreateInstance(
    CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (LPVOID*)&psl);
  if (SUCCEEDED(hres))
  {
    psl->SetPath(lnk.path.c_str());
    psl->SetArguments(lnk.argument.c_str());
    psl->SetWorkingDirectory(lnk.directory.c_str());
    psl->SetHotkey(lnk.hotkey);
    psl->SetShowCmd(lnk.window);
    psl->SetDescription(lnk.description.c_str());
    psl->SetIconLocation(lnk.icon.first.c_str(), lnk.icon.second);

    IPersistFile* ppf = nullptr;
    hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
    if (SUCCEEDED(hres))
    {
      ppf->Save(lnk_file_path.c_str(), TRUE);
      ppf->Release();
    }

    psl->Release();
  }

  CoUninitialize();

  return VU_OK;
}

#endif // LNK

} // namespace vu