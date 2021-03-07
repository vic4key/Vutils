/**
 * @file   service.cpp
 * @author Vic P.
 * @brief  Implementation for Service
 */

#include "Vutils.h"

#include <algorithm>

namespace vu
{

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 26812)
#endif // _MSC_VER

template <class StringT>
bool fnContains(const StringT& _1, const StringT& _2, bool exact)
{
  auto v1 = _1;
  std::transform(v1.begin(), v1.end(), v1.begin(), tolower);

  auto v2 = _2;
  std::transform(v2.begin(), v2.end(), v2.begin(), tolower);

  if (exact)
  {
    if (v1 == v2)
    {
      return true;
    }
  }
  else
  {
    if (v2.find(v1) != StringT::npos)
    {
      return true;
    }
  }

  return false;
};

/**
 * CServiceManagerT
 */

SMTypes CServiceManagerT<SMDeclares>::CServiceManagerT() : m_Manager(nullptr), m_Initialized(false)
{
  // this->Refresh();
}

SMTypes CServiceManagerT<SMDeclares>::~CServiceManagerT()
{
}

SMTypes void CServiceManagerT<SMDeclares>::Refresh()
{
  m_Initialized = false;
  this->Initialize();
}

SMTypes typename CServiceManagerT<SMDeclares>::TServices CServiceManagerT<SMDeclares>::GetServices(
  ulong types,
  ulong states
)
{
  if (types == SERVICE_ALL_TYPES && states == SERVICE_ALL_STATES)
  {
    return m_Services;
  }

  TServices result;

  for (const auto& e : m_Services)
  {
    if ((types & e.ServiceStatusProcess.dwServiceType) && (states & e.ServiceStatusProcess.dwCurrentState))
    {
      result.push_back(e);
    }
  }

  return result;
}

/**
 * CServiceManagerA
 */

CServiceManagerA::CServiceManagerA() : CServiceManagerTA()
{
  this->Refresh();
}

CServiceManagerA::~CServiceManagerA()
{
}

VUResult CServiceManagerA::Initialize()
{
  if (m_Initialized)
  {
    return VU_OK;
  }

  m_Services.clear();

  if (m_Manager == nullptr)
  {
    m_Manager = OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
    m_LastErrorCode = GetLastError();
  }

  if (m_Manager == nullptr)
  {
    return __LINE__;
  }

  DWORD cbBytesNeeded = 0, nServicesReturned = 0;

  EnumServicesStatusExA(
    m_Manager,
    SC_ENUM_PROCESS_INFO,
    SERVICE_TYPE_ALL,
    SERVICE_STATE_ALL,
    nullptr,
    0,
    &cbBytesNeeded,
    &nServicesReturned,
    nullptr,
    nullptr
  );

  m_LastErrorCode = GetLastError();

  if (cbBytesNeeded == 0)
  {
    return __LINE__;
  }

  m_Services.resize(cbBytesNeeded / sizeof(TServices::value_type) + 1); // +1 for padding

  EnumServicesStatusExA(
    m_Manager,
    SC_ENUM_PROCESS_INFO,
    SERVICE_TYPE_ALL,
    SERVICE_STATE_ALL,
    PBYTE(m_Services.data()),
    cbBytesNeeded,
    &cbBytesNeeded,
    &nServicesReturned,
    nullptr,
    nullptr
  );

  m_LastErrorCode = GetLastError();

  m_Services.resize(nServicesReturned);

  m_Initialized = true;

  return VU_OK;
}

std::unique_ptr<SERVICE_STATUS> CServiceManagerA::Control(
  const CServiceManagerA::TServices::value_type* pService,
  const ulong ctrlcode
)
{
  if (pService == nullptr)
  {
    return nullptr;
  }

  auto hService = OpenServiceA(m_Manager, pService->lpServiceName, SERVICE_ALL_ACCESS);

  m_LastErrorCode = GetLastError();

  if (hService == nullptr)
  {
    return nullptr;
  }

  SERVICE_STATUS ss = { 0 };
  auto ret = ControlService(hService, ctrlcode, &ss);

  m_LastErrorCode = GetLastError();

  CloseServiceHandle(hService);

  if (ret == FALSE)
  {
    return nullptr;
  }

  // TODO: Vic. Recheck. Wait for finish here.

  this->Refresh();

  std::unique_ptr<SERVICE_STATUS> result(new SERVICE_STATUS);
  *result.get() = ss;

  return result;
}

std::unique_ptr<SERVICE_STATUS> CServiceManagerA::Control(
  const std::string& name,
  const ulong ctrlcode
)
{
  auto pService = this->Query(name);
  if (pService == nullptr)
  {
    return nullptr;
  }

  return this->Control(pService.get(), ctrlcode);
}

CServiceManagerA::TServices CServiceManagerA::Find(
  const std::string& str,
  bool exact,
  bool nameonly
)
{
  TServices result;

  if (str.empty())
  {
    return result;
  }

  for (const auto& service : m_Services)
  {
    if (fnContains<std::string>(str, service.lpServiceName, exact))
    {
      result.push_back(service);
    }
    else if (!nameonly && fnContains<std::string>(str, service.lpDisplayName, exact))
    {
      result.push_back(service);
    }
  }

  return result;
}

std::unique_ptr<CServiceManagerA::TServices::value_type> CServiceManagerA::Query(
  const std::string& service_name)
{
  if (service_name.empty())
  {
    return nullptr;
  }

  auto l = this->Find(service_name, true, true);
  if (l.empty())
  {
    return nullptr;
  }

  std::unique_ptr<TServices::value_type> result(new TServices::value_type);
  *result.get() = l[0];

  return result;
}

int CServiceManagerA::GetState(const std::string& service_name)
{
  auto pService = this->Query(service_name);
  if (pService == nullptr)
  {
    return -1;
  }

  return pService->ServiceStatusProcess.dwCurrentState;
}

CServiceManagerA::TDependents CServiceManagerA::GetDependents(
  const std::string& service_name, const ulong states)
{
  TDependents result;

  auto pService = this->Query(service_name);
  if (pService == nullptr)
  {
    return result;
  }

  auto hService = OpenServiceA(m_Manager, pService->lpServiceName, SERVICE_ALL_ACCESS);

  m_LastErrorCode = GetLastError();

  if (hService == nullptr)
  {
    return result;
  }

  DWORD cbBytesNeeded = 0, nServicesReturned = 0;

  EnumDependentServicesA(
    hService,
    states,
    nullptr,
    0,
    &cbBytesNeeded,
    &nServicesReturned
  );

  m_LastErrorCode = GetLastError();

  if (cbBytesNeeded > 0)
  {
    result.resize(cbBytesNeeded / sizeof(TDependents::value_type) + 1); // +1 for padding

    EnumDependentServicesA(
      hService,
      states,
      result.data(),
      cbBytesNeeded,
      &cbBytesNeeded,
      &nServicesReturned
    );

    m_LastErrorCode = GetLastError();

    result.resize(nServicesReturned);
  }

  CloseServiceHandle(hService);

  return result;
}

VUResult CServiceManagerA::Delete(const std::string& name)
{
  auto pService = this->Query(name);
  if (pService == nullptr)
  {
    return __LINE__;
  }

  this->Stop(name);

  auto hService = OpenServiceA(m_Manager, name.c_str(), SERVICE_ALL_ACCESS);

  m_LastErrorCode = GetLastError();

  if (hService == nullptr)
  {
    return __LINE__;
  }

  BOOL result = DeleteService(hService);

  m_LastErrorCode = GetLastError();

  CloseServiceHandle(hService);

  if (result == FALSE)
  {
    return __LINE__;
  }

  this->Refresh();

  return VU_OK;
}

VUResult CServiceManagerA::Start(const std::string& name)
{
  auto pService = this->Query(name);
  if (pService == nullptr)
  {
    return __LINE__;
  }

  if (pService->ServiceStatusProcess.dwCurrentState == SERVICE_RUNNING)
  {
    return VU_OK;
  }
  else if (pService->ServiceStatusProcess.dwCurrentState == SERVICE_STOPPED)
  {
    auto hService = OpenServiceA(m_Manager, pService->lpServiceName, SERVICE_ALL_ACCESS);

    m_LastErrorCode = GetLastError();

    if (hService == nullptr)
    {
      return __LINE__;
    }

    BOOL ret = StartServiceA(hService, 0, nullptr);

    m_LastErrorCode = GetLastError();

    CloseServiceHandle(hService);

    if (ret == FALSE)
    {
      return __LINE__;
    }

    // TODO: RiGa. Recheck. Wait for finish here.

    this->Refresh();

    return VU_OK;
  }
  else
  {
    return __LINE__;
  }

  return VU_OK;
}

VUResult CServiceManagerA::Stop(const std::string& name)
{
  auto pService = this->Query(name);
  if (pService == nullptr)
  {
    return __LINE__;
  }

  if (pService->ServiceStatusProcess.dwCurrentState == SERVICE_STOPPED)
  {
    return VU_OK;
  }
  else if (pService->ServiceStatusProcess.dwCurrentState == SERVICE_RUNNING)
  {
    return this->Control(pService.get(), SERVICE_CONTROL_STOP) != nullptr ? VU_OK : __LINE__;
  }
  else
  {
    return __LINE__;
  }

  return VU_OK;
}

VUResult CServiceManagerA::Create(
  const std::string& file_path,
  const std::string& service_name,
  const std::string& display_name,
  const ulong service_type,
  const ulong access_type,
  const ulong start_type,
  const ulong ctrl_type)
{
  if (file_path.empty() || service_name.empty() || display_name.empty())
  {
    m_LastErrorCode = ERROR_BAD_ARGUMENTS;
    return __LINE__;
  }

  if (!IsFileExistsA(file_path))
  {
    m_LastErrorCode = ERROR_FILE_NOT_FOUND;
    return __LINE__;
  }

  if (this->Query(service_name) != nullptr)
  {
    m_LastErrorCode = ERROR_ALREADY_EXISTS;
    return __LINE__;
  }

  auto hService = CreateServiceA(
    m_Manager,
    service_name.c_str(),
    display_name.c_str(),
    access_type,
    service_type,
    start_type,
    ctrl_type,
    file_path.c_str(),
    NULL, NULL, NULL, NULL, NULL
  );

  m_LastErrorCode = GetLastError();

  if (hService == nullptr)
  {
    return __LINE__;
  }

  CloseServiceHandle(hService);

  this->Refresh();

  return VU_OK;
}

/**
 * CServiceManagerW
 */

CServiceManagerW::CServiceManagerW() : CServiceManagerTW()
{
  this->Refresh();
}

CServiceManagerW::~CServiceManagerW()
{
}

VUResult CServiceManagerW::Initialize()
{
  if (m_Initialized)
  {
    return VU_OK;
  }

  m_Services.clear();

  m_Manager = OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);

  m_LastErrorCode = GetLastError();

  if (m_Manager == nullptr)
  {
    return __LINE__;
  }

  DWORD cbBytesNeeded = 0, nServicesReturned = 0;

  EnumServicesStatusExW(
    m_Manager,
    SC_ENUM_PROCESS_INFO,
    SERVICE_TYPE_ALL,
    SERVICE_STATE_ALL,
    nullptr,
    0,
    &cbBytesNeeded,
    &nServicesReturned,
    nullptr,
    nullptr
  );

  m_LastErrorCode = GetLastError();

  if (cbBytesNeeded == 0)
  {
    return __LINE__;
  }

  m_Services.resize(cbBytesNeeded / sizeof(TServices::value_type) + 1); // +1 for padding

  EnumServicesStatusExW(
    m_Manager,
    SC_ENUM_PROCESS_INFO,
    SERVICE_TYPE_ALL,
    SERVICE_STATE_ALL,
    PBYTE(m_Services.data()),
    cbBytesNeeded,
    &cbBytesNeeded,
    &nServicesReturned,
    nullptr,
    nullptr
  );

  m_LastErrorCode = GetLastError();

  m_Services.resize(nServicesReturned);

  m_Initialized = true;

  return VU_OK;
}

std::unique_ptr<SERVICE_STATUS> CServiceManagerW::Control(
  const CServiceManagerW::TServices::value_type* pService,
  const ulong ctrlcode
)
{
  if (pService == nullptr)
  {
    return nullptr;
  }

  auto hService = OpenServiceW(m_Manager, pService->lpServiceName, SERVICE_ALL_ACCESS);

  m_LastErrorCode = GetLastError();

  if (hService == nullptr)
  {
    return nullptr;
  }

  SERVICE_STATUS ss = { 0 };
  auto ret = ControlService(hService, ctrlcode, &ss);

  m_LastErrorCode = GetLastError();

  CloseServiceHandle(hService);

  if (ret == FALSE)
  {
    return nullptr;
  }

  // TODO: Vic. Recheck. Wait for finish here.

  this->Refresh();

  std::unique_ptr<SERVICE_STATUS> result(new SERVICE_STATUS);
  *result.get() = ss;

  return result;
}

std::unique_ptr<SERVICE_STATUS> CServiceManagerW::Control(
  const std::wstring& name,
  const ulong ctrlcode
)
{
  auto pService = this->Query(name);
  if (pService == nullptr)
  {
    return nullptr;
  }

  return this->Control(pService.get(), ctrlcode);
}

CServiceManagerW::TServices CServiceManagerW::Find(
  const std::wstring& str,
  bool exact,
  bool nameonly
)
{
  TServices result;

  if (str.empty())
  {
    return result;
  }

  for (const auto& service : m_Services)
  {
    if (fnContains<std::wstring>(str, service.lpServiceName, exact))
    {
      result.push_back(service);
    }
    else if (!nameonly && fnContains<std::wstring>(str, service.lpDisplayName, exact))
    {
      result.push_back(service);
    }
  }

  return result;
}

std::unique_ptr<CServiceManagerW::TServices::value_type> CServiceManagerW::Query(
  const std::wstring& service_name)
{
  if (service_name.empty())
  {
    return nullptr;
  }

  auto l = this->Find(service_name, true, true);
  if (l.empty())
  {
    return nullptr;
  }

  std::unique_ptr<TServices::value_type> result(new TServices::value_type);
  *result.get() = l[0];

  return result;
}

int CServiceManagerW::GetState(const std::wstring& service_name)
{
  auto pService = this->Query(service_name);
  if (pService == nullptr)
  {
    return -1;
  }

  return pService->ServiceStatusProcess.dwCurrentState;
}

CServiceManagerW::TDependents CServiceManagerW::GetDependents(
  const std::wstring& service_name, const ulong states)
{
  TDependents result;

  auto pService = this->Query(service_name);
  if (pService == nullptr)
  {
    return result;
  }

  auto hService = OpenServiceW(m_Manager, pService->lpServiceName, SERVICE_ALL_ACCESS);

  m_LastErrorCode = GetLastError();

  if (hService == nullptr)
  {
    return result;
  }

  DWORD cbBytesNeeded = 0, nServicesReturned = 0;

  EnumDependentServicesW(
    hService,
    states,
    nullptr,
    0,
    &cbBytesNeeded,
    &nServicesReturned
  );

  m_LastErrorCode = GetLastError();

  if (cbBytesNeeded > 0)
  {
    result.resize(cbBytesNeeded / sizeof(TDependents::value_type) + 1); // +1 for padding

    EnumDependentServicesW(
      hService,
      states,
      result.data(),
      cbBytesNeeded,
      &cbBytesNeeded,
      &nServicesReturned
    );

    m_LastErrorCode = GetLastError();

    result.resize(nServicesReturned);
  }

  CloseServiceHandle(hService);

  return result;
}

VUResult CServiceManagerW::Delete(const std::wstring& name)
{
  auto pService = this->Query(name);
  if (pService == nullptr)
  {
    return __LINE__;
  }

  this->Stop(name);

  auto hService = OpenServiceW(m_Manager, name.c_str(), SERVICE_ALL_ACCESS);

  m_LastErrorCode = GetLastError();

  if (hService == nullptr)
  {
    return __LINE__;
  }

  BOOL result = DeleteService(hService);

  m_LastErrorCode = GetLastError();

  CloseServiceHandle(hService);

  if (result == FALSE)
  {
    return __LINE__;
  }

  this->Refresh();

  return VU_OK;
}

VUResult CServiceManagerW::Start(const std::wstring& name)
{
  auto pService = this->Query(name);
  if (pService == nullptr)
  {
    return __LINE__;
  }

  if (pService->ServiceStatusProcess.dwCurrentState == SERVICE_RUNNING)
  {
    return VU_OK;
  }
  else if (pService->ServiceStatusProcess.dwCurrentState == SERVICE_STOPPED)
  {
    auto hService = OpenServiceW(m_Manager, pService->lpServiceName, SERVICE_ALL_ACCESS);

    m_LastErrorCode = GetLastError();

    if (hService == nullptr)
    {
      return __LINE__;
    }

    BOOL ret = StartServiceW(hService, 0, nullptr);

    m_LastErrorCode = GetLastError();

    CloseServiceHandle(hService);

    if (ret == FALSE)
    {
      return __LINE__;
    }

    // TODO: RiGa. Recheck. Wait for finish here.

    this->Refresh();

    return VU_OK;
  }
  else
  {
    return __LINE__;
  }

  return VU_OK;
}

VUResult CServiceManagerW::Stop(const std::wstring& name)
{
  auto pService = this->Query(name);
  if (pService == nullptr)
  {
    return __LINE__;
  }

  if (pService->ServiceStatusProcess.dwCurrentState == SERVICE_STOPPED)
  {
    return VU_OK;
  }
  else if (pService->ServiceStatusProcess.dwCurrentState == SERVICE_RUNNING)
  {
    return this->Control(pService.get(), SERVICE_CONTROL_STOP) != nullptr ? VU_OK : __LINE__;
  }
  else
  {
    return __LINE__;
  }

  return VU_OK;
}

VUResult CServiceManagerW::Create(
  const std::wstring& file_path,
  const std::wstring& service_name,
  const std::wstring& display_name,
  const ulong service_type,
  const ulong access_type,
  const ulong start_type,
  const ulong ctrl_type)
{
  if (file_path.empty() || service_name.empty() || display_name.empty())
  {
    m_LastErrorCode = ERROR_BAD_ARGUMENTS;
    return __LINE__;
  }

  if (!IsFileExistsW(file_path))
  {
    m_LastErrorCode = ERROR_FILE_NOT_FOUND;
    return __LINE__;
  }

  if (this->Query(service_name) != nullptr)
  {
    m_LastErrorCode = ERROR_ALREADY_EXISTS;
    return __LINE__;
  }

  auto hService = CreateServiceW(
    m_Manager,
    service_name.c_str(),
    display_name.c_str(),
    access_type,
    service_type,
    start_type,
    ctrl_type,
    file_path.c_str(),
    NULL, NULL, NULL, NULL, NULL
  );

  m_LastErrorCode = GetLastError();

  if (hService == nullptr)
  {
    return __LINE__;
  }

  CloseServiceHandle(hService);

  this->Refresh();

  return VU_OK;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

} // namespace vu