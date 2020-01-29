/**
 * @file   service.cpp
 * @author Vic P.
 * @brief  Implementation for Service
 */

#include "Vutils.h"

namespace vu
{

bool vuapi CServiceX::Init(eSCAccessType SCAccessType)
{
  m_SCMHandle = OpenSCManager(
    NULL, // Local computer
    NULL, // ServicesActive database
    (ulong)SCAccessType
  );

  m_LastErrorCode = GetLastError();

  if (!m_SCMHandle)
  {
    m_Initialized = false;
    return false;
  }

  m_Initialized = true;

  return true;
}

bool vuapi CServiceX::Destroy()
{
  if (!m_Initialized)
  {
    return false;
  }

  BOOL result = CloseServiceHandle(m_SCMHandle);

  m_LastErrorCode = GetLastError();

  if (result == FALSE)
  {
    return false;
  }

  return true;
}

bool vuapi CServiceX::Start()
{
  if (!m_ServiceHandle)
  {
    return false;
  }

  BOOL result = ::StartServiceW(m_ServiceHandle, 0, NULL);

  m_LastErrorCode = GetLastError();

  return (result != FALSE);
}

bool vuapi CServiceX::Stop()
{
  return this->Control(SC_STOP);
}

bool vuapi CServiceX::Control(eServiceControl ServiceControl)
{
  if (!m_ServiceHandle)
  {
    return false;
  }

  BOOL result = ::ControlService(m_ServiceHandle, (ulong)ServiceControl, (LPSERVICE_STATUS)&m_Status);

  m_LastErrorCode = GetLastError();

  return (result != FALSE);
}

bool vuapi CServiceX::Close()
{
  if (!m_ServiceHandle)
  {
    return false;
  }

  BOOL result = DeleteService(m_ServiceHandle);

  m_LastErrorCode = GetLastError();

  if (result == FALSE)
  {
    return false;
  }

  result = CloseServiceHandle(m_ServiceHandle);

  m_LastErrorCode = GetLastError();

  return (result != FALSE);
}

bool vuapi CServiceX::QueryStatus(TServiceStatus& ServiceStatus)
{
  if (!m_ServiceHandle)
  {
    return false;
  }

  BOOL result = ::QueryServiceStatus(m_ServiceHandle, &ServiceStatus);

  m_LastErrorCode = GetLastError();

  if (result == FALSE)
  {
    return false;
  }

  return true;
}

eServiceType vuapi CServiceX::GetType()
{
  if (!m_ServiceHandle)
  {
    return eServiceType::ST_UNKNOWN;
  }

  if (this->QueryStatus(m_Status) == FALSE)
  {
    return eServiceType::ST_UNKNOWN;
  }

  return (vu::eServiceType)m_Status.dwServiceType;
}

eServiceState vuapi CServiceX::GetState()
{
  if (!m_ServiceHandle)
  {
    return eServiceState::SS_UNKNOWN;
  }

  if (this->QueryStatus(m_Status) == FALSE)
  {
    return eServiceState::SS_UNKNOWN;
  }

  return (vu::eServiceState)m_Status.dwCurrentState;
}

// A

CServiceA::CServiceA()
{
  m_ServiceName.clear();
  m_DisplayName.clear();
  m_ServiceFilePath.clear();

  m_SCMHandle     = 0;
  m_ServiceHandle = 0;

  m_LastErrorCode = ERROR_SUCCESS;
}

CServiceA::~CServiceA()
{
  m_SCMHandle     = 0;
  m_ServiceHandle = 0;
}

bool vuapi CServiceA::Create (
  const std::string ServiceFilePath,
  eServiceAccessType ServiceAccessType,
  eServiceType ServiceType,
  eServiceStartType ServiceStartType,
  eServiceErrorControlType ServiceErrorControlType
)
{
  // Is a full file path?
  if (ServiceFilePath.find('\\') == std::string::npos)
  {
    m_ServiceFilePath = GetCurrentDirectoryA(true);
    m_ServiceFilePath.append(ServiceFilePath);
  }
  else
  {
    m_ServiceFilePath.assign(ServiceFilePath);
  }

  if (!IsFileExistsA(m_ServiceFilePath))
  {
    SetLastError(ERROR_FILE_NOT_FOUND);
    return false;
  }

  m_ServiceFileName = ExtractFileNameA(m_ServiceFilePath);

  m_ServiceName = ExtractFileNameA(m_ServiceFilePath, false);

  time_t t = time(NULL);
  std::string crTime = FormatDateTimeA(t, "%H:%M:%S");
  std::string crDateTime = FormatDateTimeA(t, "%H:%M:%S %d/%m/%Y");

  // Name
  m_Name.clear();
  m_Name.assign(m_ServiceName);
  m_Name.append(" - ");
  m_Name.append(crTime);

  // Display Name
  m_DisplayName.clear();
  m_DisplayName.assign(m_ServiceName);
  m_DisplayName.append(" - ");
  m_DisplayName.append(crDateTime);

  m_ServiceHandle = ::CreateServiceA(
    m_SCMHandle,
    m_Name.c_str(),
    m_DisplayName.c_str(),
    (ulong)ServiceAccessType,
    (ulong)ServiceType,
    (ulong)ServiceStartType,
    (ulong)ServiceErrorControlType,
    m_ServiceFilePath.c_str(),
    NULL, NULL, NULL, NULL, NULL
  );

  m_LastErrorCode = GetLastError();

  if (!m_ServiceHandle)
  {
    CloseServiceHandle(m_SCMHandle);
    return false;
  }

  return true;
}

bool vuapi CServiceA::Open(const std::string& ServiceName, eServiceAccessType ServiceAccessType)
{
  if (!m_SCMHandle)
  {
    return false;
  }

  m_ServiceName = ServiceName;

  m_ServiceHandle = ::OpenServiceA(m_SCMHandle, m_ServiceName.c_str(), (ulong)ServiceAccessType);

  m_LastErrorCode = GetLastError();

  if (!m_ServiceHandle)
  {
    return false;
  }

  return true;
}

std::string vuapi CServiceA::GetName(const std::string& AnotherServiceDisplayName)
{
  if (AnotherServiceDisplayName.empty())
  {
    return m_Name;
  }

  std::string s;
  s.clear();
  ulong nSize = MAX_SIZE;

  std::unique_ptr<char[]> p(new char [MAX_SIZE]);
  ZeroMemory(p.get(), MAX_SIZE);

  BOOL result = GetServiceKeyNameA(m_SCMHandle, AnotherServiceDisplayName.c_str(), p.get(), &nSize);

  m_LastErrorCode = GetLastError();

  if (result == FALSE)
  {
    return s;
  }

  s.assign(p.get());

  return s;
}

std::string vuapi CServiceA::GetDisplayName(const std::string& AnotherServiceName)
{
  if (AnotherServiceName.empty())
  {
    return m_DisplayName;
  }

  std::string s;
  s.clear();
  ulong nSize = MAX_SIZE;

  std::unique_ptr<char[]> p(new char [MAX_SIZE]);
  ZeroMemory(p.get(), MAX_SIZE);

  BOOL result = GetServiceDisplayNameA(m_SCMHandle, AnotherServiceName.c_str(), p.get(), &nSize);

  m_LastErrorCode = GetLastError();

  if (result == FALSE)
  {
    return s;
  }

  s.assign(p.get());

  return s;
}

// W

CServiceW::CServiceW()
{
  m_ServiceName.clear();
  m_DisplayName.clear();
  m_ServiceFilePath.clear();

  m_SCMHandle     = 0;
  m_ServiceHandle = 0;

  m_LastErrorCode = ERROR_SUCCESS;
}

CServiceW::~CServiceW()
{
  m_SCMHandle     = 0;
  m_ServiceHandle = 0;
}

bool vuapi CServiceW::Create(
  const std::wstring& ServiceFilePath,
  eServiceAccessType ServiceAccessType,
  eServiceType ServiceType,
  eServiceStartType ServiceStartType,
  eServiceErrorControlType ServiceErrorControlType
)
{
  // Is a full file path?
  if (ServiceFilePath.find(L'\\') == std::wstring::npos)
  {
    m_ServiceFilePath = GetCurrentDirectoryW(true);
    m_ServiceFilePath.append(ServiceFilePath);
  }
  else
  {
    m_ServiceFilePath.assign(ServiceFilePath);
  }

  if (!IsFileExistsW(m_ServiceFilePath))
  {
    return false;
  }

  m_ServiceFileName = ExtractFileNameW(m_ServiceFilePath);

  m_ServiceName = ExtractFileNameW(m_ServiceFilePath, false);

  time_t t = time(NULL);
  std::wstring crTime = FormatDateTimeW(t, L"%H:%M:%S");
  std::wstring crDateTime = FormatDateTimeW(t, L"%H:%M:%S %d/%m/%Y");

  // Name
  m_Name.clear();
  m_Name.assign(m_ServiceName);
  m_Name.append(L" - ");
  m_Name.append(crTime);

  // Display Name
  m_DisplayName.clear();
  m_DisplayName.assign(m_ServiceName);
  m_DisplayName.append(L" - ");
  m_DisplayName.append(crDateTime);

  m_ServiceHandle = ::CreateServiceW(
    m_SCMHandle,
    m_Name.c_str(),
    m_DisplayName.c_str(),
    (ulong)ServiceAccessType,
    (ulong)ServiceType,
    (ulong)ServiceStartType,
    (ulong)ServiceErrorControlType,
    m_ServiceFilePath.c_str(),
    NULL, NULL, NULL, NULL, NULL
  );

  m_LastErrorCode = GetLastError();

  if (!m_ServiceHandle)
  {
    return false;
  }

  return true;
}

bool vuapi CServiceW::Open(const std::wstring& ServiceName, eServiceAccessType ServiceAccessType)
{
  if (!m_SCMHandle)
  {
    return false;
  }

  m_ServiceName = ServiceName;

  m_ServiceHandle = ::OpenServiceW(m_SCMHandle, m_ServiceName.c_str(), (ulong)ServiceAccessType);

  m_LastErrorCode = GetLastError();

  if (!m_ServiceHandle)
  {
    return false;
  }

  return true;
}

std::wstring vuapi CServiceW::GetName(const std::wstring& AnotherServiceDisplayName)
{
  if (AnotherServiceDisplayName.empty())
  {
    return m_Name;
  }

  std::wstring s;
  s.clear();
  ulong nSize = 2*MAX_SIZE;

  std::unique_ptr<wchar[]> p(new wchar [MAX_SIZE]);
  ZeroMemory(p.get(), 2*MAX_SIZE);

  BOOL result = GetServiceKeyNameW(m_SCMHandle, AnotherServiceDisplayName.c_str(), p.get(), &nSize);

  m_LastErrorCode = GetLastError();

  if (result == FALSE)
  {
    return s;
  }

  s.assign(p.get());

  return s;
}

std::wstring vuapi CServiceW::GetDisplayName(const std::wstring& AnotherServiceName)
{
  if (AnotherServiceName.empty())
  {
    return m_DisplayName;
  }

  std::wstring s;
  s.clear();
  ulong nSize = 2*MAX_SIZE;

  std::unique_ptr<wchar[]> p(new wchar [MAX_SIZE]);
  ZeroMemory(p.get(), 2*MAX_SIZE);

  BOOL result = GetServiceDisplayNameW(m_SCMHandle, AnotherServiceName.c_str(), p.get(), &nSize);

  m_LastErrorCode = GetLastError();

  if (result == FALSE)
  {
    return s;
  }

  s.assign(p.get());

  return s;
}

} // namespace vu