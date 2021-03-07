#pragma once

#include "Vutils.h"

#include <conio.h>

#include <vu>
#include <algorithm>

using namespace vu;

DEF_SAMPLE(ServiceManager)
{
  if (!vu::IsAdministrator())
  {
    std::tcout << _T("You are not Administrator") << std::endl;
  }
  else
  {
    std::tstring driver_name = _T("WKE Driver");
    std::tstring driver_display_name = _T("Windows Kernel Explorer Driver");
    std::tstring driver_path = vu::GetCurDirectory();
    #ifdef _WIN64
    driver_path += _T("WKE64.sys");
    #else // _WIN32
    driver_path += _T("WKE32.sys");
    #endif // _WIN64

    CServiceManager::Instance().Create(driver_path, driver_name, driver_display_name);

    auto services = vu::CServiceManager::Instance().GetServices(SERVICE_ALL_TYPES, SERVICE_RUNNING);
    assert(!services.empty());

    auto pService = vu::CServiceManager::Instance().Query(driver_name);
    assert(pService != nullptr);
    
    auto l = CServiceManager::Instance().Find(driver_name);
    for (auto& e : l)
    {
      std::tcout << e.lpServiceName << _T(" - ") << e.lpDisplayName << std::endl;
    }

    std::tcout << _T("Press any key to start service ...") << std::endl; _getch();
    CServiceManager::Instance().Start(driver_name);
    vu::CServiceManager::Instance().GetState(driver_name);
    
    std::tcout << _T("Press any key to stop service ...") << std::endl; _getch();
    vu::CServiceManager::Instance().Stop(driver_name);
    vu::CServiceManager::Instance().GetState(driver_name);

    std::tcout << _T("Press any key to delete service ...") << std::endl; _getch();
    CServiceManager::Instance().Delete(driver_name);

    auto l = CServiceManager::Instance().GetDependents(_T("WSearch"));
    for (auto& e : l)
    {
      std::tcout << e.lpServiceName << _T(" - ") << e.lpDisplayName << std::endl;
    }
  }

  return vu::VU_OK;
}