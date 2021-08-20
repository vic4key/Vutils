#pragma once

#include "Sample.h"

DEF_SAMPLE(Registry)
{
  vu::CRegistry reg(vu::HKLM, ts("SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting")); // ts("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\Windows Error Reporting")
  if (!reg.KeyExists())
  {
    std::tcout << ts("Reg -> Exist -> Failed") << vu::get_last_error(reg.get_last_error_code()) << std::endl;
    return 1;
  }

  if (!reg.OpenKey())
  {
    std::tcout << ts("Reg -> Open-> Failed") << vu::get_last_error(reg.get_last_error_code()) << std::endl;
    return 1;
  }

  reg.SetReflectionKey(vu::eRegReflection::RR_ENABLE);

  std::tcout << ts("Is Reflection Disabled ? ")
    << (reg.QueryReflectionKey() == vu::eRegReflection::RR_DISABLED ? ts("True") : ts("False"))
    << std::endl;

  std::vector<std::tstring> l;

  std::tcout << ts("\n[ErrorPort] -> ");
  std::tstring result = reg.ReadString(ts("ErrorPort"), ts("<No>"));
  std::tcout << result << std::endl;

  std::tcout << ts("\n[MS]") << std::endl;
  l.clear();
  l = reg.ReadMultiString(ts("MS"), l);
  for (auto e : l)
  {
    std::tcout << e << std::endl;
  }

  std::tcout << ts("\n[Keys]") << std::endl;
  l.clear();
  l = reg.EnumKeys();
  for (auto e : l)
  {
    std::tcout << e << std::endl;
  }

  std::tcout << ts("\n[Values]") << std::endl;
  l.clear();
  l = reg.EnumValues();
  for (auto e : l)
  {
    std::tcout << e << std::endl;
  }

  std::tcout << std::endl;

  reg.WriteBinary(ts("RegBinary"), (void*)ts("1234567890"), 10 * sizeof(vu::tchar));
  std::tcout << ts("RegBinary\t") << (vu::tchar*)reg.ReadBinary(ts("RegBinary"), nullptr).get() << std::endl;

  reg.WriteBool(ts("RegBool"), true);
  std::tcout << ts("RegBool\t") << reg.ReadBool(ts("RegBool"), false) << std::endl;

  reg.WriteExpandString(ts("RegExpandString"), ts("%TMP%"));
  std::tcout << ts("RegExpandString\t") << reg.ReadExpandString(ts("RegExpandString"), ts("")) << std::endl;

  reg.WriteFloat(ts("RegFloat"), 16.09F);
  std::tcout << ts("RegFloat\t") << reg.ReadFloat(ts("RegFloat"), 7.02F) << std::endl;

  reg.WriteInteger(ts("RegInt"), 1609);
  std::tcout << ts("RegInt\t") << reg.ReadInteger(ts("RegInt"), 702) << std::endl;

  l.clear();
  l.push_back(ts("String 1"));
  l.push_back(ts("String 2"));
  l.push_back(ts("String 3"));
  reg.WriteMultiString(ts("RegMultiString"), l);
  l.clear();
  std::tcout << ts("RegMultiString") << std::endl;
  l = reg.ReadMultiString(ts("RegMultiString"), l);
  for (auto e : l) {
    std::tcout << ts("\t") << e << std::endl;
  }

  reg.WriteString(ts("RegString"), ts("This is a string"));
  std::tcout << ts("RegString\t") << reg.ReadString(ts("RegString"), ts("")) << std::endl;

  if (!reg.CloseKey())
  {
    std::tcout << ts("Reg -> Close ->Failed") << vu::get_last_error(reg.get_last_error_code()) << std::endl;
  }

  return vu::VU_OK;
}
