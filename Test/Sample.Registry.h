#pragma once

#include "Sample.h"

DEF_SAMPLE(Registry)
{
  vu::CRegistry reg(vu::HKLM, _T("SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting")); // _T("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\Windows Error Reporting")
  if (!reg.KeyExists())
  {
    std::tcout << _T("Reg -> Exist -> Failed") << vu::LastError(reg.GetLastErrorCode()) << std::endl;
    return 1;
  }

  if (!reg.OpenKey())
  {
    std::tcout << _T("Reg -> Open-> Failed") << vu::LastError(reg.GetLastErrorCode()) << std::endl;
    return 1;
  }

  reg.SetReflectionKey(vu::eRegReflection::RR_ENABLE);

  std::tcout << _T("Is Reflection Disabled ? ")
    << (reg.QueryReflectionKey() == vu::eRegReflection::RR_DISABLED ? _T("True") : _T("False"))
    << std::endl;

  std::vector<std::tstring> l;

  std::tcout << _T("\n[ErrorPort] -> ");
  std::tstring result = reg.ReadString(_T("ErrorPort"), _T("<No>"));
  std::tcout << result << std::endl;

  std::tcout << _T("\n[MS]") << std::endl;
  l.clear();
  l = reg.ReadMultiString(_T("MS"), l);
  for (auto e : l)
  {
    std::tcout << e << std::endl;
  }

  std::tcout << _T("\n[Keys]") << std::endl;
  l.clear();
  l = reg.EnumKeys();
  for (auto e : l)
  {
    std::tcout << e << std::endl;
  }

  std::tcout << _T("\n[Values]") << std::endl;
  l.clear();
  l = reg.EnumValues();
  for (auto e : l)
  {
    std::tcout << e << std::endl;
  }

  std::tcout << std::endl;

  reg.WriteBinary(_T("RegBinary"), (void*)_T("1234567890"), 10 * sizeof(vu::tchar));
  std::tcout << _T("RegBinary\t") << (vu::tchar*)reg.ReadBinary(_T("RegBinary"), nullptr).get() << std::endl;

  reg.WriteBool(_T("RegBool"), true);
  std::tcout << _T("RegBool\t") << reg.ReadBool(_T("RegBool"), false) << std::endl;

  reg.WriteExpandString(_T("RegExpandString"), _T("%TMP%"));
  std::tcout << _T("RegExpandString\t") << reg.ReadExpandString(_T("RegExpandString"), _T("")) << std::endl;

  reg.WriteFloat(_T("RegFloat"), 16.09F);
  std::tcout << _T("RegFloat\t") << reg.ReadFloat(_T("RegFloat"), 7.02F) << std::endl;

  reg.WriteInteger(_T("RegInt"), 1609);
  std::tcout << _T("RegInt\t") << reg.ReadInteger(_T("RegInt"), 702) << std::endl;

  l.clear();
  l.push_back(_T("String 1"));
  l.push_back(_T("String 2"));
  l.push_back(_T("String 3"));
  reg.WriteMultiString(_T("RegMultiString"), l);
  l.clear();
  std::tcout << _T("RegMultiString") << std::endl;
  l = reg.ReadMultiString(_T("RegMultiString"), l);
  for (auto e : l) {
    std::tcout << _T("\t") << e << std::endl;
  }

  reg.WriteString(_T("RegString"), _T("This is a string"));
  std::tcout << _T("RegString\t") << reg.ReadString(_T("RegString"), _T("")) << std::endl;

  if (!reg.CloseKey())
  {
    std::tcout << _T("Reg -> Close ->Failed") << vu::LastError(reg.GetLastErrorCode()) << std::endl;
  }

  return vu::VU_OK;
}
