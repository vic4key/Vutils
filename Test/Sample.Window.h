#pragma once

#include "Sample.h"

#include <cassert>

DEF_SAMPLE(Window)
{
  auto pids = vu::name_to_pid(ts("notepad.exe"));
  if (!pids.empty())
  {
    auto pid = pids.back();

    auto hwnd_top_win = vu::find_top_window(pid);
    std::tcout << ts("top window  : ") << std::hex << hwnd_top_win << std::endl;

    auto hwnd_main_win = vu::find_main_window(hwnd_top_win);
    std::tcout << ts("main window : ") << std::hex << hwnd_main_win << std::endl;

    auto font = vu::get_font(hwnd_top_win);
    std::tcout << ts("font : ") << font.name << ts(" - ") << font.size << std::endl;

    vu::Monitors monitors;
    vu::get_monitors(monitors);
    for (auto& monitor : monitors)
    {
      std::tcout << monitor.szDevice << std::endl;
    }
  }

  return vu::VU_OK;
}