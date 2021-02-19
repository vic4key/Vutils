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

  virtual bool Ready();

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

bool CCOMSentry::Ready()
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

  virtual bool Ready();
  bool Begin(const std::wstring& theObjectPath);
  bool End();

  IEnumWbemClassObject* Query(const std::wstring& theQueryString);
  bool Query(const std::wstring& theQueryString, const std::function<bool(IWbemClassObject& object)> fnCallback);

private:
  bool SetupWBEM(const std::wstring& theObjectPath);

private:
  IWbemLocator* m_pWbemLocator;
  IWbemServices* m_pWbemServices;
};

CWMIProviderX::CWMIProviderX() : CCOMSentry(), m_pWbemLocator(nullptr), m_pWbemServices(nullptr)
{
}

CWMIProviderX::~CWMIProviderX()
{
}

bool CWMIProviderX::Ready()
{
  return CCOMSentry::Ready() && m_pWbemServices != nullptr && m_pWbemLocator != nullptr;
}

bool CWMIProviderX::Begin(const std::wstring& theObjectPath)
{
  if (!CCOMSentry::Ready())
  {
    return false;
  }

  return SetupWBEM(theObjectPath);
}

bool CWMIProviderX::End()
{
  if (m_pWbemServices != nullptr)
  {
    m_pWbemServices->Release();
    m_pWbemServices = nullptr;
  }

  if (m_pWbemLocator != nullptr)
  {
    m_pWbemLocator->Release();
    m_pWbemLocator = nullptr;
  }

  return true;
}

bool CWMIProviderX::SetupWBEM(const std::wstring& theObjectPath)
{
  // Obtain the initial locater to WMI

  HRESULT ret = CoCreateInstance(
    CLSID_WbemLocator,
    0,
    CLSCTX_INPROC_SERVER,
    IID_IWbemLocator,
    (LPVOID*)&m_pWbemLocator
  );
  if (FAILED(ret))
  {
    return false;
  }

  // Connect to WMI

  BSTR bstrObjectPath = SysAllocString(theObjectPath.c_str());

  ret = m_pWbemLocator->ConnectServer(
    bstrObjectPath, // Object path of WMI namespace
    NULL, // User name
    NULL, // User password
    0,    // Locale
    0,    // Security
    0,    // Authority
    0,    // Context
    &m_pWbemServices // pointer to IWbemServices proxy
  );

  SysFreeString(bstrObjectPath);

  if (FAILED(ret))
  {
    m_pWbemLocator->Release();
    return false;
  }

  // Set security levels on the proxy

  ret = CoSetProxyBlanket(
    m_pWbemServices,             // Indicates the proxy to set
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
    m_pWbemServices->Release();
    m_pWbemLocator->Release();
    return false;
  }

  return true;
}

IEnumWbemClassObject* CWMIProviderX::Query(const std::wstring& theQueryString)
{
  if (!Ready() || theQueryString.empty())
  {
    return nullptr;
  }

  IEnumWbemClassObject* pEnumerator = nullptr;

  BSTR bstrWQL = SysAllocString(L"WQL");
  BSTR bstrSQL = SysAllocString(theQueryString.c_str());

  HRESULT ret = m_pWbemServices->ExecQuery(
    bstrWQL,
    bstrSQL,
    WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
    NULL,
    &pEnumerator
  );

  SysFreeString(bstrSQL);
  SysFreeString(bstrWQL);

  if (FAILED(ret))
  {
    return nullptr;
  }

  return pEnumerator;
}

bool CWMIProviderX::Query(const std::wstring& theQueryString,
                         const std::function<bool(IWbemClassObject& object)> fnCallback)
{
  auto pEnumerator = Query(theQueryString);
  while (pEnumerator)
  {
    ULONG returned = 0;
    IWbemClassObject* pObject = nullptr;

    HRESULT ret = pEnumerator->Next(WBEM_INFINITE, 1, &pObject, &returned);
    if (FAILED(ret) || pObject == nullptr || returned == 0)
    {
      break;
    }

    if (!fnCallback(*pObject))
    {
      break;
    }

    pObject->Release();
  }

  return true;
}

/**
 * CWMIProviderA
 */

CWMIProviderA::CWMIProviderA()
{
  m_pImpl = new CWMIProviderX();
}

CWMIProviderA::~CWMIProviderA()
{
  delete m_pImpl;
}

bool CWMIProviderA::Ready()
{
  return m_pImpl->Ready();
}

bool CWMIProviderA::Begin(const std::string& ns)
{
  auto s = vu::ToStringW(ns);
  return m_pImpl->Begin(s);
}

bool CWMIProviderA::End()
{
  return m_pImpl->End();
}

IEnumWbemClassObject* CWMIProviderA::Query(const std::string& qs)
{
  auto s = vu::ToStringW(qs);
  return m_pImpl->Query(s);
}

bool CWMIProviderA::Query(const std::string& qs, const std::function<bool(IWbemClassObject& object)> fn)
{
  auto s = vu::ToStringW(qs);
  return m_pImpl->Query(s, fn);
}

/**
 * CWMIProviderW
 */

CWMIProviderW::CWMIProviderW()
{
  m_pImpl = new CWMIProviderX();
}

CWMIProviderW::~CWMIProviderW()
{
  delete m_pImpl;
}

bool CWMIProviderW::Ready()
{
  return m_pImpl->Ready();
}

bool CWMIProviderW::Begin(const std::wstring& ns)
{
  return m_pImpl->Begin(ns);
}

bool CWMIProviderW::End()
{
  return m_pImpl->End();
}

IEnumWbemClassObject* CWMIProviderW::Query(const std::wstring& qs)
{
  return m_pImpl->Query(qs);
}

bool CWMIProviderW::Query(const std::wstring& qs, const std::function<bool(IWbemClassObject& object)> fn)
{
  return m_pImpl->Query(qs, fn);
}

#endif // VU_WMI_ENABLED

} // namespace vu