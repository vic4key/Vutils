/**
 * @file   wmi.cpp
 * @author Vic P.
 * @brief  Implementation for Windows Management Instrumentation
 */

#include "Vutils.h"

#include <comdef.h>
#include <wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")

namespace vu
{

/**
 * Implementation for Component Object Model a.k.a COM
 */

CCOMSentry::CCOMSentry() : m_Ready(false)
{
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
    return;
  }

  m_Ready = true;
}

CCOMSentry::~CCOMSentry()
{
  CoUninitialize();
}

bool CCOMSentry::Ready()
{
  return m_Ready;
}

/**
 * Implementation for Windows Management Instrumentation a.k.a WMI
 */

CWMIProvider::CWMIProvider() : CCOMSentry(), m_pWbemLocator(nullptr), m_pWbemServices(nullptr)
{
}

CWMIProvider::~CWMIProvider()
{
}

bool CWMIProvider::Ready()
{
  return __super::Ready() && m_pWbemServices != nullptr && m_pWbemLocator != nullptr;
}

bool CWMIProvider::Begin(const std::wstring& theObjectPath)
{
  if (!__super::Ready())
  {
    return false;
  }

  return SetupWBEM(theObjectPath);
}

bool CWMIProvider::End()
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

bool CWMIProvider::SetupWBEM(const std::wstring& theObjectPath)
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

  ret = m_pWbemLocator->ConnectServer(
    _bstr_t(theObjectPath.c_str()), // Object path of WMI namespace
    NULL, // User name
    NULL, // User password
    0,    // Locale
    NULL, // Security
    0,    // Authority
    0,    // Context
    &m_pWbemServices // pointer to IWbemServices proxy
  );
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

IEnumWbemClassObject* CWMIProvider::Query(const std::wstring& theQueryString)
{
  if (!Ready() || theQueryString.empty())
  {
    return nullptr;
  }

  IEnumWbemClassObject* pEnumerator = nullptr;

  HRESULT ret = m_pWbemServices->ExecQuery(
    bstr_t("WQL"),
    bstr_t(theQueryString.c_str()),
    WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
    NULL,
    &pEnumerator
  );
  if (FAILED(ret))
  {
    return nullptr;
  }

  return pEnumerator;
}

bool CWMIProvider::Query(const std::wstring& theQueryString,
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

} // namespace vu