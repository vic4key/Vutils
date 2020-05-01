#pragma once

#include "Sample.h"

DEF_SAMPLE(INIFile)
{
  vu::CINIFile ini(vu::GetCurrentFilePath() + _T(".ini"));

  ini.SetCurrentSection(_T("Section"));

  std::vector<std::tstring> l;
  l.clear();

  l = ini.ReadSectionNames();
  for (auto i = l.begin(); i != l.end(); i++)
  {
    _tprintf(_T("[%s]\n"), (*i).c_str());
  }

  std::cout << std::endl;

  l = ini.ReadSection();
  for (auto i = l.begin(); i != l.end(); i++)
  {
    _tprintf(_T("[%s]\n"), (*i).c_str());
  }

  printf("\n");

  if (ini.WriteInteger(_T("KeyInt"), 702))
  {
    _tprintf(_T("KeyInt = %d\n"), ini.ReadInteger(_T("KeyInt"), 0));
  }

  if (ini.WriteBool(_T("KeyBool"), true))
  {
    _tprintf(_T("KeyBool = %s\n"), ini.ReadBool(_T("KeyBool"), 0) ? _T("True") : _T("False"));
  }

  if (ini.WriteFloat(_T("KeyFloat"), 7.02f))
  {
    _tprintf(_T("KeyFloat = %.2f\n"), ini.ReadFloat(_T("KeyFloat"), 0.F));
  }

  if (ini.WriteString(_T("KeyString"), _T("Vic P.")))
  {
    std::tstring s = ini.ReadString(_T("KeyString"), _T(""));
    _tprintf(_T("KeyString = '%s'\n"), s.c_str());
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

  if (ini.WriteStruct(_T("KeyStruct"), &Input, sizeof(Input)))
  {
    std::shared_ptr<void> p = ini.ReadStruct(_T("KeyStruct"), sizeof(TStruct));
    TStruct* Output = (TStruct*)p.get();
    _tprintf(_T("Value = [%c, %d, %.2f]\n"), Output->a, Output->b, Output->c);
  }

  return vu::VU_OK;
}
