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

template <typename ServiceT>
CServiceManagerT<ServiceT>::CServiceManagerT() : m_hmanager(nullptr), m_initialized(false)
{
  // this->Refresh();
}

template <typename ServiceT>
CServiceManagerT<ServiceT>::~CServiceManagerT()
{
}

template <typename ServiceT>
void CServiceManagerT<ServiceT>::refresh()
{
  m_initialized = false;
  this->initialize();
}

template <typename ServiceT>
typename CServiceManagerT<ServiceT>::TServices CServiceManagerT<ServiceT>::get_services(
  ulong types,
  ulong states
)
{
  if (types == VU_SERVICE_ALL_TYPES && states == VU_SERVICE_ALL_STATES)
  {
    return m_services;
  }

  TServices result;

  for (const auto& e : m_services)
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
  this->refresh();
}

CServiceManagerA::~CServiceManagerA()
{
}

VUResult CServiceManagerA::initialize()
{
  if (m_initialized)
  {
    return VU_OK;
  }

  m_services.clear();

  if (m_hmanager == nullptr)
  {
    m_hmanager = OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
    m_last_error_code = GetLastError();
  }

  if (m_hmanager == nullptr)
  {
    return __LINE__;
  }

  DWORD cb_bytes_needed = 0, n_services_returned = 0;

  EnumServicesStatusExA(
    m_hmanager,
    SC_ENUM_PROCESS_INFO,
    SERVICE_TYPE_ALL,
    SERVICE_STATE_ALL,
    nullptr,
    0,
    &cb_bytes_needed,
    &n_services_returned,
    nullptr,
    nullptr
  );

  m_last_error_code = GetLastError();

  if (cb_bytes_needed == 0)
  {
    return __LINE__;
  }

  m_services.resize(cb_bytes_needed / sizeof(TServices::value_type) + 1); // +1 for padding

  EnumServicesStatusExA(
    m_hmanager,
    SC_ENUM_PROCESS_INFO,
    SERVICE_TYPE_ALL,
    SERVICE_STATE_ALL,
    PBYTE(m_services.data()),
    cb_bytes_needed,
    &cb_bytes_needed,
    &n_services_returned,
    nullptr,
    nullptr
  );

  m_last_error_code = GetLastError();

  m_services.resize(n_services_returned);

  m_initialized = true;

  return VU_OK;
}

std::unique_ptr<SERVICE_STATUS> CServiceManagerA::control(
  const CServiceManagerA::TServices::value_type* pService,
  const ulong ctrlcode
)
{
  if (pService == nullptr)
  {
    return nullptr;
  }

  auto hService = OpenServiceA(m_hmanager, pService->lpServiceName, SERVICE_ALL_ACCESS);

  m_last_error_code = GetLastError();

  if (hService == nullptr)
  {
    return nullptr;
  }

  SERVICE_STATUS ss = { 0 };
  auto ret = ControlService(hService, ctrlcode, &ss);

  m_last_error_code = GetLastError();

  CloseServiceHandle(hService);

  if (ret == FALSE)
  {
    return nullptr;
  }

  // TODO: Vic. Recheck. Wait for finish here.

  this->refresh();

  std::unique_ptr<SERVICE_STATUS> result(new SERVICE_STATUS);
  *result.get() = ss;

  return result;
}

std::unique_ptr<SERVICE_STATUS> CServiceManagerA::control(
  const std::string& name,
  const ulong ctrlcode
)
{
  auto pservice = this->query(name);
  if (pservice == nullptr)
  {
    return nullptr;
  }

  return this->control(pservice.get(), ctrlcode);
}

CServiceManagerA::TServices CServiceManagerA::find(
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

  for (const auto& service : m_services)
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

std::unique_ptr<CServiceManagerA::TServices::value_type> CServiceManagerA::query(
  const std::string& service_name)
{
  if (service_name.empty())
  {
    return nullptr;
  }

  auto l = this->find(service_name, true, true);
  if (l.empty())
  {
    return nullptr;
  }

  std::unique_ptr<TServices::value_type> result(new TServices::value_type);
  *result.get() = l[0];

  return result;
}

int CServiceManagerA::get_state(const std::string& service_name)
{
  auto ptr_service = this->query(service_name);
  if (ptr_service == nullptr)
  {
    return -1;
  }

  return ptr_service->ServiceStatusProcess.dwCurrentState;
}

CServiceManagerA::TServices CServiceManagerA::get_dependents(
  const std::string& service_name, const ulong states)
{
  TServices result;

  auto ptr_service = this->query(service_name);
  if (ptr_service == nullptr)
  {
    return result;
  }

  auto hservice = OpenServiceA(m_hmanager, ptr_service->lpServiceName, SERVICE_ALL_ACCESS);

  m_last_error_code = GetLastError();

  if (hservice == nullptr)
  {
    return result;
  }

  DWORD cb_bytes_needed = 0, n_services_returned = 0;

  EnumDependentServicesA(
    hservice,
    states == VU_SERVICE_ALL_STATES ? SERVICE_STATE_ALL : states,
    nullptr,
    0,
    &cb_bytes_needed,
    &n_services_returned
  );

  m_last_error_code = GetLastError();

  if (cb_bytes_needed > 0)
  {
    std::vector<ENUM_SERVICE_STATUSA> dependents;
    dependents.resize(cb_bytes_needed / sizeof(ENUM_SERVICE_STATUSA) + 1); // +1 for zero padding

    EnumDependentServicesA(
      hservice,
      states == VU_SERVICE_ALL_STATES ? SERVICE_STATE_ALL : states,
      dependents.data(),
      cb_bytes_needed,
      &cb_bytes_needed,
      &n_services_returned
    );

    m_last_error_code = GetLastError();

    dependents.resize(n_services_returned);

    for (const auto& dependent : dependents)
    {
      auto pService = this->query(dependent.lpServiceName);
      if (pService != nullptr)
      {
        result.push_back(*pService);
      }
    }
  }

  CloseServiceHandle(hservice);

  return result;
}

CServiceManagerA::TServices CServiceManagerA::get_dependencies(
  const std::string& service_name, const ulong states)
{
  TServices result;

  auto pService = this->query(service_name);
  if (pService == nullptr)
  {
    return result;
  }

  auto hService = OpenServiceA(m_hmanager, pService->lpServiceName, SERVICE_ALL_ACCESS);

  m_last_error_code = GetLastError();

  if (hService == nullptr)
  {
    return result;
  }

  DWORD cb_bytes_needed = 0;

  QueryServiceConfigA(hService, nullptr, 0, &cb_bytes_needed);

  m_last_error_code = GetLastError();

  if (cb_bytes_needed > 0)
  {
    std::vector<char> buffer(cb_bytes_needed);
    auto ptr = LPQUERY_SERVICE_CONFIGA(buffer.data());

    QueryServiceConfigA(hService, ptr, cb_bytes_needed, &cb_bytes_needed);

    m_last_error_code = GetLastError();

    const auto dependencies = multi_string_to_list_A(ptr->lpDependencies);
    for (const auto& dependency : dependencies)
    {
      auto pService = this->query(dependency);
      if (pService != nullptr)
      {
        if (states & pService->ServiceStatusProcess.dwCurrentState)
        {
          result.push_back(*pService);
        }
      }
    }
  }

  CloseServiceHandle(hService);

  return result;
}

VUResult CServiceManagerA::uninstall(const std::string& name)
{
  auto ptr_service = this->query(name);
  if (ptr_service == nullptr)
  {
    return __LINE__;
  }

  this->stop(name);

  auto hservice = OpenServiceA(m_hmanager, name.c_str(), SERVICE_ALL_ACCESS);

  m_last_error_code = GetLastError();

  if (hservice == nullptr)
  {
    return __LINE__;
  }

  BOOL result = DeleteService(hservice);

  m_last_error_code = GetLastError();

  CloseServiceHandle(hservice);

  if (result == FALSE)
  {
    return __LINE__;
  }

  this->refresh();

  return VU_OK;
}

VUResult CServiceManagerA::start(const std::string& name)
{
  auto ptr_service = this->query(name);
  if (ptr_service == nullptr)
  {
    return __LINE__;
  }

  if (ptr_service->ServiceStatusProcess.dwCurrentState == SERVICE_RUNNING)
  {
    return VU_OK;
  }
  else if (ptr_service->ServiceStatusProcess.dwCurrentState == SERVICE_STOPPED)
  {
    auto hservice = OpenServiceA(m_hmanager, ptr_service->lpServiceName, SERVICE_ALL_ACCESS);

    m_last_error_code = GetLastError();

    if (hservice == nullptr)
    {
      return __LINE__;
    }

    BOOL ret = StartServiceA(hservice, 0, nullptr);

    m_last_error_code = GetLastError();

    CloseServiceHandle(hservice);

    if (ret == FALSE)
    {
      return __LINE__;
    }

    // TODO: Vic. Recheck. Wait for finish here.

    this->refresh();

    return VU_OK;
  }
  else
  {
    return __LINE__;
  }

  return VU_OK;
}

VUResult CServiceManagerA::stop(const std::string& name)
{
  auto ptr_service = this->query(name);
  if (ptr_service == nullptr)
  {
    return __LINE__;
  }

  if (ptr_service->ServiceStatusProcess.dwCurrentState == SERVICE_STOPPED)
  {
    return VU_OK;
  }
  else if (ptr_service->ServiceStatusProcess.dwCurrentState == SERVICE_RUNNING)
  {
    return this->control(ptr_service.get(), SERVICE_CONTROL_STOP) != nullptr ? VU_OK : __LINE__;
  }
  else
  {
    return __LINE__;
  }

  return VU_OK;
}

VUResult CServiceManagerA::install(
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
    m_last_error_code = ERROR_BAD_ARGUMENTS;
    return __LINE__;
  }

  if (!is_file_exists_A(file_path))
  {
    m_last_error_code = ERROR_FILE_NOT_FOUND;
    return __LINE__;
  }

  if (this->query(service_name) != nullptr)
  {
    m_last_error_code = ERROR_ALREADY_EXISTS;
    return __LINE__;
  }

  auto hservice = CreateServiceA(
    m_hmanager,
    service_name.c_str(),
    display_name.c_str(),
    access_type,
    service_type,
    start_type,
    ctrl_type,
    file_path.c_str(),
    NULL, NULL, NULL, NULL, NULL
  );

  m_last_error_code = GetLastError();

  if (hservice == nullptr)
  {
    return __LINE__;
  }

  CloseServiceHandle(hservice);

  this->refresh();

  return VU_OK;
}

/**
 * CServiceManagerW
 */

CServiceManagerW::CServiceManagerW() : CServiceManagerTW()
{
  this->refresh();
}

CServiceManagerW::~CServiceManagerW()
{
}

VUResult CServiceManagerW::initialize()
{
  if (m_initialized)
  {
    return VU_OK;
  }

  m_services.clear();

  m_hmanager = OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);

  m_last_error_code = GetLastError();

  if (m_hmanager == nullptr)
  {
    return __LINE__;
  }

  DWORD cb_bytes_needed = 0, n_services_returned = 0;

  EnumServicesStatusExW(
    m_hmanager,
    SC_ENUM_PROCESS_INFO,
    SERVICE_TYPE_ALL,
    SERVICE_STATE_ALL,
    nullptr,
    0,
    &cb_bytes_needed,
    &n_services_returned,
    nullptr,
    nullptr
  );

  m_last_error_code = GetLastError();

  if (cb_bytes_needed == 0)
  {
    return __LINE__;
  }

  m_services.resize(cb_bytes_needed / sizeof(TServices::value_type) + 1); // +1 for zero padding

  EnumServicesStatusExW(
    m_hmanager,
    SC_ENUM_PROCESS_INFO,
    SERVICE_TYPE_ALL,
    SERVICE_STATE_ALL,
    PBYTE(m_services.data()),
    cb_bytes_needed,
    &cb_bytes_needed,
    &n_services_returned,
    nullptr,
    nullptr
  );

  m_last_error_code = GetLastError();

  m_services.resize(n_services_returned);

  m_initialized = true;

  return VU_OK;
}

std::unique_ptr<SERVICE_STATUS> CServiceManagerW::control(
  const CServiceManagerW::TServices::value_type* pService,
  const ulong ctrlcode
)
{
  if (pService == nullptr)
  {
    return nullptr;
  }

  auto hservice = OpenServiceW(m_hmanager, pService->lpServiceName, SERVICE_ALL_ACCESS);

  m_last_error_code = GetLastError();

  if (hservice == nullptr)
  {
    return nullptr;
  }

  SERVICE_STATUS ss = { 0 };
  auto ret = ControlService(hservice, ctrlcode, &ss);

  m_last_error_code = GetLastError();

  CloseServiceHandle(hservice);

  if (ret == FALSE)
  {
    return nullptr;
  }

  // TODO: Vic. Recheck. Wait for finish here.

  this->refresh();

  std::unique_ptr<SERVICE_STATUS> result(new SERVICE_STATUS);
  *result.get() = ss;

  return result;
}

std::unique_ptr<SERVICE_STATUS> CServiceManagerW::control(
  const std::wstring& name,
  const ulong ctrlcode
)
{
  auto pService = this->query(name);
  if (pService == nullptr)
  {
    return nullptr;
  }

  return this->control(pService.get(), ctrlcode);
}

CServiceManagerW::TServices CServiceManagerW::find(
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

  for (const auto& service : m_services)
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

std::unique_ptr<CServiceManagerW::TServices::value_type> CServiceManagerW::query(
  const std::wstring& service_name)
{
  if (service_name.empty())
  {
    return nullptr;
  }

  auto l = this->find(service_name, true, true);
  if (l.empty())
  {
    return nullptr;
  }

  std::unique_ptr<TServices::value_type> result(new TServices::value_type);
  *result.get() = l[0];

  return result;
}

int CServiceManagerW::get_state(const std::wstring& service_name)
{
  auto pService = this->query(service_name);
  if (pService == nullptr)
  {
    return -1;
  }

  return pService->ServiceStatusProcess.dwCurrentState;
}

CServiceManagerW::TServices CServiceManagerW::get_dependents(
  const std::wstring& service_name, const ulong states)
{
  TServices result;

  auto ptr_service = this->query(service_name);
  if (ptr_service == nullptr)
  {
    return result;
  }

  auto hService = OpenServiceW(m_hmanager, ptr_service->lpServiceName, SERVICE_ALL_ACCESS);

  m_last_error_code = GetLastError();

  if (hService == nullptr)
  {
    return result;
  }

  DWORD cb_bytes_needed = 0, n_services_returned = 0;

  EnumDependentServicesW(
    hService,
    states == VU_SERVICE_ALL_STATES ? SERVICE_STATE_ALL : states,
    nullptr,
    0,
    &cb_bytes_needed,
    &n_services_returned
  );

  m_last_error_code = GetLastError();

  if (cb_bytes_needed > 0)
  {
    std::vector<ENUM_SERVICE_STATUSW> dependents;
    dependents.resize(cb_bytes_needed / sizeof(ENUM_SERVICE_STATUSW) + 1); // +1 for padding

    EnumDependentServicesW(
      hService,
      states == VU_SERVICE_ALL_STATES ? SERVICE_STATE_ALL : states,
      dependents.data(),
      cb_bytes_needed,
      &cb_bytes_needed,
      &n_services_returned
    );

    m_last_error_code = GetLastError();

    dependents.resize(n_services_returned);

    for (const auto& dependent : dependents)
    {
      auto pService = this->query(dependent.lpServiceName);
      if (pService != nullptr)
      {
        result.push_back(*pService);
      }
    }
  }

  CloseServiceHandle(hService);

  return result;
}

CServiceManagerW::TServices CServiceManagerW::get_dependencies(
  const std::wstring& service_name, const ulong states)
{
  TServices result;

  auto pService = this->query(service_name);
  if (pService == nullptr)
  {
    return result;
  }

  auto hservice = OpenServiceW(m_hmanager, pService->lpServiceName, SERVICE_ALL_ACCESS);

  m_last_error_code = GetLastError();

  if (hservice == nullptr)
  {
    return result;
  }

  DWORD cb_bytes_needed = 0;

  QueryServiceConfigW(hservice, nullptr, 0, &cb_bytes_needed);

  m_last_error_code = GetLastError();

  if (cb_bytes_needed > 0)
  {
    std::vector<char> buffer(cb_bytes_needed);
    auto ptr = LPQUERY_SERVICE_CONFIGW(buffer.data());

    QueryServiceConfigW(hservice, ptr, cb_bytes_needed, &cb_bytes_needed);

    m_last_error_code = GetLastError();

    const auto dependencies = multi_string_to_list_W(ptr->lpDependencies);
    for (const auto& dependency : dependencies)
    {
      auto pService = this->query(dependency);
      if (pService != nullptr)
      {
        if (states & pService->ServiceStatusProcess.dwCurrentState)
        {
          result.push_back(*pService);
        }
      }
    }
  }

  CloseServiceHandle(hservice);

  return result;
}

VUResult CServiceManagerW::uninstall(const std::wstring& name)
{
  auto ptr_service = this->query(name);
  if (ptr_service == nullptr)
  {
    return __LINE__;
  }

  this->stop(name);

  auto hservice = OpenServiceW(m_hmanager, name.c_str(), SERVICE_ALL_ACCESS);

  m_last_error_code = GetLastError();

  if (hservice == nullptr)
  {
    return __LINE__;
  }

  BOOL result = DeleteService(hservice);

  m_last_error_code = GetLastError();

  CloseServiceHandle(hservice);

  if (result == FALSE)
  {
    return __LINE__;
  }

  this->refresh();

  return VU_OK;
}

VUResult CServiceManagerW::start(const std::wstring& name)
{
  auto ptr_service = this->query(name);
  if (ptr_service == nullptr)
  {
    return __LINE__;
  }

  if (ptr_service->ServiceStatusProcess.dwCurrentState == SERVICE_RUNNING)
  {
    return VU_OK;
  }
  else if (ptr_service->ServiceStatusProcess.dwCurrentState == SERVICE_STOPPED)
  {
    auto hservice = OpenServiceW(m_hmanager, ptr_service->lpServiceName, SERVICE_ALL_ACCESS);

    m_last_error_code = GetLastError();

    if (hservice == nullptr)
    {
      return __LINE__;
    }

    BOOL ret = StartServiceW(hservice, 0, nullptr);

    m_last_error_code = GetLastError();

    CloseServiceHandle(hservice);

    if (ret == FALSE)
    {
      return __LINE__;
    }

    // TODO: Vic. Recheck. Wait for finish here.

    this->refresh();

    return VU_OK;
  }
  else
  {
    return __LINE__;
  }

  return VU_OK;
}

VUResult CServiceManagerW::stop(const std::wstring& name)
{
  auto ptr_service = this->query(name);
  if (ptr_service == nullptr)
  {
    return __LINE__;
  }

  if (ptr_service->ServiceStatusProcess.dwCurrentState == SERVICE_STOPPED)
  {
    return VU_OK;
  }
  else if (ptr_service->ServiceStatusProcess.dwCurrentState == SERVICE_RUNNING)
  {
    return this->control(ptr_service.get(), SERVICE_CONTROL_STOP) != nullptr ? VU_OK : __LINE__;
  }
  else
  {
    return __LINE__;
  }

  return VU_OK;
}

VUResult CServiceManagerW::install(
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
    m_last_error_code = ERROR_BAD_ARGUMENTS;
    return __LINE__;
  }

  if (!is_file_exists_W(file_path))
  {
    m_last_error_code = ERROR_FILE_NOT_FOUND;
    return __LINE__;
  }

  if (this->query(service_name) != nullptr)
  {
    m_last_error_code = ERROR_ALREADY_EXISTS;
    return __LINE__;
  }

  auto hservice = CreateServiceW(
    m_hmanager,
    service_name.c_str(),
    display_name.c_str(),
    access_type,
    service_type,
    start_type,
    ctrl_type,
    file_path.c_str(),
    NULL, NULL, NULL, NULL, NULL
  );

  m_last_error_code = GetLastError();

  if (hservice == nullptr)
  {
    return __LINE__;
  }

  CloseServiceHandle(hservice);

  this->refresh();

  return VU_OK;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

} // namespace vu