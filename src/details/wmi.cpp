/**
 * @file   wmi.cpp
 * @author Vic P.
 * @brief  Implementation for Windows Management Instrumentation
 */

#include "Vutils.h"

#ifdef VU_WMI_ENABLED
#include <comdef.h>
#include <wbemidl.h>
#if defined(_MSC_VER) || defined(__BCPLUSPLUS__)
#pragma comment(lib, "wbemuuid.lib")
#endif
#endif // VU_WMI_ENABLED

namespace vu
{

#ifdef VU_WMI_ENABLED

/**
 * Implementation for Component Object Model a.k.a COM
 */

/**
 * CCOMSentry
 */

class CCOMSentry
{
public:
  CCOMSentry();
  virtual ~CCOMSentry();

  virtual bool ready();

  // private:
  static bool m_Ready;
};

bool CCOMSentry::m_Ready = false;

CCOMSentry::CCOMSentry()
{
  if (m_Ready)
  {
    return;
  }

  HRESULT ret = CoInitializeEx(0, COINIT_MULTITHREADED);
  if (FAILED(ret))
  {
    return;
  }

  ret = CoInitializeSecurity(
    NULL,
    -1,                          // COM authentication
    NULL,                        // Authentication services
    NULL,                        // Reserved
    RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
    RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
    NULL,                        // Authentication info
    EOAC_NONE,                   // Additional capabilities
    NULL                         // Reserved
  );
  if (FAILED(ret))
  {
    if (ret != RPC_E_TOO_LATE)
    {
      return;
    }
  }

  m_Ready = true;
}

CCOMSentry::~CCOMSentry()
{
  if (m_Ready)
  {
    CoUninitialize();
  }

  m_Ready = false;
}

bool CCOMSentry::ready()
{
  return m_Ready;
}

/**
 * Implementation for Windows Management Instrumentation a.k.a WMI
 */

/**
 * CWMIProviderX
 */

class CWMIProviderX : public CCOMSentry
{
public:
  CWMIProviderX();
  virtual ~CWMIProviderX();

  virtual bool ready();
  bool begin(const std::wstring& object_path);
  bool end();

  IEnumWbemClassObject* query(const std::wstring& qs);
  bool query(const std::wstring& qs, const std::function<bool(IWbemClassObject& object)> fn_callback);

private:
  bool setup_WBEM(const std::wstring& object_path);

private:
  IWbemLocator*  m_ptr_WbemLocator;
  IWbemServices* m_ptr_WbemServices;
};

CWMIProviderX::CWMIProviderX() : CCOMSentry(), m_ptr_WbemLocator(nullptr), m_ptr_WbemServices(nullptr)
{
}

CWMIProviderX::~CWMIProviderX()
{
}

bool CWMIProviderX::ready()
{
  return CCOMSentry::ready() && m_ptr_WbemServices != nullptr && m_ptr_WbemLocator != nullptr;
}

bool CWMIProviderX::begin(const std::wstring& object_path)
{
  if (!CCOMSentry::ready())
  {
    return false;
  }

  return setup_WBEM(object_path);
}

bool CWMIProviderX::end()
{
  if (m_ptr_WbemServices != nullptr)
  {
    m_ptr_WbemServices->Release();
    m_ptr_WbemServices = nullptr;
  }

  if (m_ptr_WbemLocator != nullptr)
  {
    m_ptr_WbemLocator->Release();
    m_ptr_WbemLocator = nullptr;
  }

  return true;
}

bool CWMIProviderX::setup_WBEM(const std::wstring& object_path)
{
  // Obtain the initial locater to WMI

  HRESULT ret = CoCreateInstance(
    CLSID_WbemLocator,
    0,
    CLSCTX_INPROC_SERVER,
    IID_IWbemLocator,
    (LPVOID*)&m_ptr_WbemLocator
  );
  if (FAILED(ret))
  {
    return false;
  }

  // Connect to WMI

  BSTR bstr_object_path = SysAllocString(object_path.c_str());

  ret = m_ptr_WbemLocator->ConnectServer(
    bstr_object_path, // Object path of WMI namespace
    NULL, // User name
    NULL, // User password
    0,    // Locale
    0,    // Security
    0,    // Authority
    0,    // Context
    &m_ptr_WbemServices // pointer to IWbemServices proxy
  );

  SysFreeString(bstr_object_path);

  if (FAILED(ret))
  {
    m_ptr_WbemLocator->Release();
    return false;
  }

  // Set security levels on the proxy

  ret = CoSetProxyBlanket(
    m_ptr_WbemServices,             // Indicates the proxy to set
    RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
    RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
    NULL,                        // Server principal name 
    RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
    RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
    NULL,                        // client identity
    EOAC_NONE                    // proxy capabilities 
  );

  if (FAILED(ret))
  {
    m_ptr_WbemServices->Release();
    m_ptr_WbemLocator->Release();
    return false;
  }

  return true;
}

IEnumWbemClassObject* CWMIProviderX::query(const std::wstring& qs)
{
  if (!ready() || qs.empty())
  {
    return nullptr;
  }

  IEnumWbemClassObject* ptr_enumerator = nullptr;

  BSTR bstr_wgl = SysAllocString(L"WQL");
  BSTR bstr_sql = SysAllocString(qs.c_str());

  HRESULT ret = m_ptr_WbemServices->ExecQuery(
    bstr_wgl,
    bstr_sql,
    WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
    NULL,
    &ptr_enumerator
  );

  SysFreeString(bstr_sql);
  SysFreeString(bstr_wgl);

  if (FAILED(ret))
  {
    return nullptr;
  }

  return ptr_enumerator;
}

bool CWMIProviderX::query(const std::wstring& qs,
                          const std::function<bool(IWbemClassObject& object)> fnCallback)
{
  auto ptr_enumerator = this->query(qs);
  while (ptr_enumerator)
  {
    ULONG returned = 0;
    IWbemClassObject* ptr_object = nullptr;

    HRESULT ret = ptr_enumerator->Next(WBEM_INFINITE, 1, &ptr_object, &returned);
    if (FAILED(ret) || ptr_object == nullptr || returned == 0)
    {
      break;
    }

    if (!fnCallback(*ptr_object))
    {
      break;
    }

    ptr_object->Release();
  }

  return true;
}

/**
 * CWMIProviderA
 */

CWMIProviderA::CWMIProviderA()
{
  m_ptr_impl = new CWMIProviderX();
}

CWMIProviderA::~CWMIProviderA()
{
  delete m_ptr_impl;
}

bool CWMIProviderA::ready()
{
  return m_ptr_impl->ready();
}

bool CWMIProviderA::begin(const std::string& ns)
{
  auto s = vu::to_string_W(ns);
  return m_ptr_impl->begin(s);
}

bool CWMIProviderA::end()
{
  return m_ptr_impl->end();
}

IEnumWbemClassObject* CWMIProviderA::query(const std::string& qs)
{
  auto s = vu::to_string_W(qs);
  return m_ptr_impl->query(s);
}

bool CWMIProviderA::query(const std::string& qs, const std::function<bool(IWbemClassObject& object)> fn)
{
  auto s = vu::to_string_W(qs);
  return m_ptr_impl->query(s, fn);
}

/**
 * CWMIProviderW
 */

CWMIProviderW::CWMIProviderW()
{
  m_ptr_impl = new CWMIProviderX();
}

CWMIProviderW::~CWMIProviderW()
{
  delete m_ptr_impl;
}

bool CWMIProviderW::ready()
{
  return m_ptr_impl->ready();
}

bool CWMIProviderW::begin(const std::wstring& ns)
{
  return m_ptr_impl->begin(ns);
}

bool CWMIProviderW::end()
{
  return m_ptr_impl->end();
}

IEnumWbemClassObject* CWMIProviderW::query(const std::wstring& qs)
{
  return m_ptr_impl->query(qs);
}

bool CWMIProviderW::query(const std::wstring& qs, const std::function<bool(IWbemClassObject& object)> fn)
{
  return m_ptr_impl->query(qs, fn);
}

#endif // VU_WMI_ENABLED

} // namespace vu