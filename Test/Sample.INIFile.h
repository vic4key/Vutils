#pragma once

#include "Sample.h"

DEF_SAMPLE(INIFile)
{
  vu::CINIFile ini(vu::get_current_file_path() + ts(".ini"));

  ini.set_current_section(ts("Section"));

  std::vector<std::tstring> l;
  l.clear();

  l = ini.read_section_names();
  for (auto i = l.begin(); i != l.end(); i++)
  {
    _tprintf(ts("[%s]\n"), (*i).c_str());
  }

  std::cout << std::endl;

  l = ini.read_current_section();
  for (auto i = l.begin(); i != l.end(); i++)
  {
    _tprintf(ts("[%s]\n"), (*i).c_str());
  }

  printf("\n");

  if (ini.write_integer(ts("KeyInt"), 702))
  {
    _tprintf(ts("KeyInt = %d\n"), ini.read_integer(ts("KeyInt"), 0));
  }

  if (ini.write_bool(ts("KeyBool"), true))
  {
    _tprintf(ts("KeyBool = %s\n"), ini.read_bool(ts("KeyBool"), 0) ? ts("True") : ts("False"));
  }

  if (ini.write_float(ts("KeyFloat"), 7.02f))
  {
    _tprintf(ts("KeyFloat = %.2f\n"), ini.read_float(ts("KeyFloat"), 0.F));
  }

  if (ini.write_string(ts("KeyString"), ts("Vic P.")))
  {
    std::tstring s = ini.read_string(ts("KeyString"), ts(""));
    _tprintf(ts("KeyString = '%s'\n"), s.c_str());
  }

  struct TStruct
  {
    char a;
    int b;
    float c;
  } Input =
  {
    'X',
    702,
    7.02f
  };

  if (ini.write_struct(ts("KeyStruct"), &Input, sizeof(Input)))
  {
    std::shared_ptr<void> p = ini.read_struct(ts("KeyStruct"), sizeof(TStruct));
    TStruct* Output = (TStruct*)p.get();
    _tprintf(ts("Value = [%c, %d, %.2f]\n"), Output->a, Output->b, Output->c);
  }

  return vu::VU_OK;
}
