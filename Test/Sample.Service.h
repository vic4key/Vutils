#pragma once

#include "Sample.h"

#include <conio.h>

DEF_SAMPLE(Service)
{
  if (!vu::IsAdministrator())
  {
    std::tcout << _T("You are not Administrator") << std::endl;
  }
  else
  {
    std::tcout << _T("You are Administrator") << std::endl;

    // CService
    vu::CService srv;

    if (srv.Init())
    {
      std::tcout << _T("Service -> Initialize -> Success") << std::endl;
    }
    else
    {
      std::tcout << _T("Service -> Initialize -> Failure") << vu::LastError() << std::endl;
    }

    std::cout << std::endl;

    std::tcout << _T("Enter to create service...") << std::endl; _getch();

    if (srv.Create(_T("C:\\Windows\\System32\\drivers\\VBoxUSBMon.sys")))
    {
      std::tcout << _T("Service -> Create -> Success") << std::endl;
    }
    else
    {
      std::tcout << _T("Service -> Create -> Failure") << vu::LastError() << std::endl;
    }

    std::cout << std::endl;

    std::tcout << _T("Enter to start service...") << std::endl; _getch();

    if (srv.Start())
    {
      std::tcout << _T("Service -> Start -> Success") << std::endl;
    }
    else
    {
      std::tcout << _T("Service -> Start -> Failure") << vu::LastError() << std::endl;
    }

    std::cout << std::endl;

    std::tcout << _T("Enter to stop service...") << std::endl; _getch();

    if (srv.Stop())
    {
      std::tcout << _T("Service -> Stop -> Success") << std::endl;
    }
    else
    {
      std::tcout << _T("Service -> Stop -> Failure") << vu::LastError() << std::endl;
    }

    std::cout << std::endl;

    std::tcout << _T("Enter to close service...") << std::endl; _getch();

    if (srv.Close())
    {
      std::tcout << _T("Service -> Close -> Success") << std::endl;
    }
    else
    {
      std::tcout << _T("Service -> Close -> Failure") << vu::LastError() << std::endl;
    }

    std::cout << std::endl;

    if (srv.Destroy())
    {
      std::tcout << _T("Service -> Release -> Success") << std::endl;
    }
    else
    {
      std::tcout << _T("Service -> Release -> Failure") << vu::LastError() << std::endl;
    }

    std::cout << std::endl;
  }

  return vu::VU_OK;
}
