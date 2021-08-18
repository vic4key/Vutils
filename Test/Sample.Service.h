#pragma once

#include "Vutils.h"

#include <conio.h>

#include <vu>
#include <algorithm>

using namespace vu;

DEF_SAMPLE(ServiceManager)
{
  if (!vu::is_administrator())
  {
    std::tcout << _T("You are not Administrator") << std::endl;
  }
  else
  {
    std::tstring driver_name = _T("WKE Driver");
    std::tstring driver_display_name = _T("Windows Kernel Explorer Driver");
    std::tstring driver_path = vu::get_current_directory();
    #ifdef _WIN64
    driver_path += _T("WKE64.sys");
    #else // _WIN32
    driver_path += _T("WKE32.sys");
    #endif // _WIN64

    // Create / Start / Stop / Delete

    std::tcout << _T("Press any key to create service ...") << std::endl; _getch();
    CServiceManager::Instance().Create(driver_path, driver_name, driver_display_name);

    std::tcout << _T("Press any key to start service ...") << std::endl; _getch();
    CServiceManager::Instance().Start(driver_name);
    
    std::tcout << _T("Press any key to stop service ...") << std::endl; _getch();
    vu::CServiceManager::Instance().Stop(driver_name);

    std::tcout << _T("Press any key to delete service ...") << std::endl; _getch();
    CServiceManager::Instance().Delete(driver_name);

    // Dependents / Dependencies

    std::tstring example = _T("WSearch");

    std::tcout << example << std::endl;

    std::tcout << _T("*Dependents:") << std::endl;

    auto dependents = CServiceManager::Instance().GetDependents(example);
    for (auto& dependent : dependents)
    {
      std::tcout << _T("  ")
        << dependent.lpServiceName << _T(" - ") << dependent.lpDisplayName << std::endl;
    }

    std::tcout << _T("*Dependencies:") << std::endl;

    auto dependencies = CServiceManager::Instance().GetDependencies(example);
    for (auto& dependency : dependencies)
    {
      std::tcout << _T("  ")
        << dependency.lpServiceName << _T(" - ") << dependency.lpDisplayName << std::endl;
    }

    // List Services

    std::tcout << _T("*Services:") << std::endl;

    auto pService = vu::CServiceManager::Instance().Query(example);
    assert(pService != nullptr);

    auto services = vu::CServiceManager::Instance().GetServices(VU_SERVICE_ALL_TYPES, SERVICE_RUNNING);
    for (auto& e : services)
    {
      std::tcout << _T("  ")
        << e.lpServiceName << _T(" - ") << e.lpDisplayName << std::endl;
    }
  }

  return vu::VU_OK;
}