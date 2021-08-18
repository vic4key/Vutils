#pragma once

#include "Sample.h"

DEF_SAMPLE(INIFile)
{
  vu::CINIFile ini(vu::get_current_file_path() + ts(".ini"));

  ini.SetCurrentSection(ts("Section"));

  std::vector<std::tstring> l;
  l.clear();

  l = ini.ReadSectionNames();
  for (auto i = l.begin(); i != l.end(); i++)
  {
    _tprintf(ts("[%s]\n"), (*i).c_str());
  }

  std::cout << std::endl;

  l = ini.ReadSection();
  for (auto i = l.begin(); i != l.end(); i++)
  {
    _tprintf(ts("[%s]\n"), (*i).c_str());
  }

  printf("\n");

  if (ini.WriteInteger(ts("KeyInt"), 702))
  {
    _tprintf(ts("KeyInt = %d\n"), ini.ReadInteger(ts("KeyInt"), 0));
  }

  if (ini.WriteBool(ts("KeyBool"), true))
  {
    _tprintf(ts("KeyBool = %s\n"), ini.ReadBool(ts("KeyBool"), 0) ? ts("True") : ts("False"));
  }

  if (ini.WriteFloat(ts("KeyFloat"), 7.02f))
  {
    _tprintf(ts("KeyFloat = %.2f\n"), ini.ReadFloat(ts("KeyFloat"), 0.F));
  }

  if (ini.WriteString(ts("KeyString"), ts("Vic P.")))
  {
    std::tstring s = ini.ReadString(ts("KeyString"), ts(""));
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

  if (ini.WriteStruct(ts("KeyStruct"), &Input, sizeof(Input)))
  {
    std::shared_ptr<void> p = ini.ReadStruct(ts("KeyStruct"), sizeof(TStruct));
    TStruct* Output = (TStruct*)p.get();
    _tprintf(ts("Value = [%c, %d, %.2f]\n"), Output->a, Output->b, Output->c);
  }

  return vu::VU_OK;
}
