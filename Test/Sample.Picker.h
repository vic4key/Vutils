#pragma once

#include "Sample.h"

DEF_SAMPLE(Picker)
{
  vu::Picker picker;

  COLORREF color = 0x00FF00;
  picker.choose_rgb_color(color);
  std::tcout << ts("COLOR -> ") << std::hex << color << std::endl;

  LOGFONT font = { 0 };
  picker.choose_log_font(font);
  std::tcout << ts("FONT -> ") << font.lfFaceName << std::endl;

  std::tstring file_dir  = ts("C:\\Windows\\");
  std::tstring file_path = ts("example_default_file_name");
  const auto file_filter = ts("All Files (*.*)\0*.*\0Document (.doc)\0*.doc\0Text (.txt)\0*.txt\0");

  picker.choose_file(vu::Picker::action_t::open, file_path, file_dir, file_filter);
  std::tcout << ts("PATH -> ") << file_path << std::endl;

  picker.choose_file(vu::Picker::action_t::save, file_path, file_dir, file_filter);
  std::tcout << ts("PATH -> ") << file_path << std::endl;
  
  std::vector<std::tstring> file_names;
  picker.choose_files(file_names, file_dir);
  std::tcout << ts("DIR -> ") << file_dir << std::endl;
  for (const auto& file_name : file_names)
  {
    std::tcout << vu::ttab << file_name << std::endl;
  }
  
  picker.choose_directory(file_dir);
  std::tcout << ts("DIR -> ") << file_dir << std::endl;

  return vu::VU_OK;
}
