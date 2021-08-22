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

class COMSentry
{
public:
  COMSentry();
  virtual ~COMSentry();

  virtual bool ready();

  // private:
  static bool m_ready;
};

bool COMSentry::m_ready = false;

COMSentry::COMSentry()
{
  if (m_ready)
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

  m_ready = true;
}

COMSentry::~COMSentry()
{
  if (m_ready)
  {
    CoUninitialize();
  }

  m_ready = false;
}

bool COMSentry::ready()
{
  return m_ready;
}

/**
 * Implementation for Windows Management Instrumentation a.k.a WMI
 */

/**
 * CWMIProviderX
 */

class WMIProviderX : public COMSentry
{
public:
  WMIProviderX();
  virtual ~WMIProviderX();

  virtual bool ready();
  bool begin(const std::wstring& object_path);
  bool end();

  IEnumWbemClassObject* query(const std::wstring& qs);
  bool query(const std::wstring& qs, const std::function<bool(IWbemClassObject& object)> fn);

private:
  bool setup_WBEM(const std::wstring& object_path);

private:
  IWbemLocator*  m_ptr_WbemLocator;
  IWbemServices* m_ptr_WbemServices;
};

WMIProviderX::WMIProviderX() : COMSentry(), m_ptr_WbemLocator(nullptr), m_ptr_WbemServices(nullptr)
{
}

WMIProviderX::~WMIProviderX()
{
}

bool WMIProviderX::ready()
{
  return COMSentry::ready() && m_ptr_WbemServices != nullptr && m_ptr_WbemLocator != nullptr;
}

bool WMIProviderX::begin(const std::wstring& object_path)
{
  if (!COMSentry::ready())
  {
    return false;
  }

  return setup_WBEM(object_path);
}

bool WMIProviderX::end()
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

bool WMIProviderX::setup_WBEM(const std::wstring& object_path)
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

IEnumWbemClassObject* WMIProviderX::query(const std::wstring& qs)
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

bool WMIProviderX::query(const std::wstring& qs,
                          const std::function<bool(IWbemClassObject& object)> fn_callback)
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

    if (!fn_callback(*ptr_object))
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

WMIProviderA::WMIProviderA()
{
  m_ptr_impl = new WMIProviderX();
}

WMIProviderA::~WMIProviderA()
{
  delete m_ptr_impl;
}

bool WMIProviderA::ready()
{
  return m_ptr_impl->ready();
}

bool WMIProviderA::begin(const std::string& ns)
{
  auto s = vu::to_string_W(ns);
  return m_ptr_impl->begin(s);
}

bool WMIProviderA::end()
{
  return m_ptr_impl->end();
}

IEnumWbemClassObject* WMIProviderA::query(const std::string& qs)
{
  auto s = vu::to_string_W(qs);
  return m_ptr_impl->query(s);
}

bool WMIProviderA::query(const std::string& qs, const std::function<bool(IWbemClassObject& object)> fn)
{
  auto s = vu::to_string_W(qs);
  return m_ptr_impl->query(s, fn);
}

/**
 * CWMIProviderW
 */

WMIProviderW::WMIProviderW()
{
  m_ptr_impl = new WMIProviderX();
}

WMIProviderW::~WMIProviderW()
{
  delete m_ptr_impl;
}

bool WMIProviderW::ready()
{
  return m_ptr_impl->ready();
}

bool WMIProviderW::begin(const std::wstring& ns)
{
  return m_ptr_impl->begin(ns);
}

bool WMIProviderW::end()
{
  return m_ptr_impl->end();
}

IEnumWbemClassObject* WMIProviderW::query(const std::wstring& qs)
{
  return m_ptr_impl->query(qs);
}

bool WMIProviderW::query(const std::wstring& qs, const std::function<bool(IWbemClassObject& object)> fn)
{
  return m_ptr_impl->query(qs, fn);
}

#endif // VU_WMI_ENABLED

} // namespace vu