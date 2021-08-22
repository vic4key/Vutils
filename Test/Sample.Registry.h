#pragma once

#include "Sample.h"

DEF_SAMPLE(Registry)
{
  vu::Registry reg(vu::HKLM, ts("SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting")); // ts("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\Windows Error Reporting")
  if (!reg.key_exists())
  {
    std::tcout << ts("Reg -> Exist -> Failed") << vu::get_last_error(reg.get_last_error_code()) << std::endl;
    return 1;
  }

  if (!reg.open_key())
  {
    std::tcout << ts("Reg -> Open-> Failed") << vu::get_last_error(reg.get_last_error_code()) << std::endl;
    return 1;
  }

  reg.set_reflection_key(vu::eRegReflection::RR_ENABLE);

  std::tcout << ts("Is Reflection Disabled ? ")
    << (reg.query_reflection_key() == vu::eRegReflection::RR_DISABLED ? ts("True") : ts("False"))
    << std::endl;

  std::vector<std::tstring> l;

  std::tcout << ts("\n[ErrorPort] -> ");
  std::tstring result = reg.read_string(ts("ErrorPort"), ts("<No>"));
  std::tcout << result << std::endl;

  std::tcout << ts("\n[MS]") << std::endl;
  l.clear();
  l = reg.read_multi_string(ts("MS"), l);
  for (auto e : l)
  {
    std::tcout << e << std::endl;
  }

  std::tcout << ts("\n[Keys]") << std::endl;
  l.clear();
  l = reg.enum_keys();
  for (auto e : l)
  {
    std::tcout << e << std::endl;
  }

  std::tcout << ts("\n[Values]") << std::endl;
  l.clear();
  l = reg.enum_values();
  for (auto e : l)
  {
    std::tcout << e << std::endl;
  }

  std::tcout << std::endl;

  reg.write_binary(ts("RegBinary"), (void*)ts("1234567890"), 10 * sizeof(vu::tchar));
  std::tcout << ts("RegBinary\t") << (vu::tchar*)reg.read_binary(ts("RegBinary"), nullptr).get() << std::endl;

  reg.write_bool(ts("RegBool"), true);
  std::tcout << ts("RegBool\t") << reg.read_bool(ts("RegBool"), false) << std::endl;

  reg.write_expand_string(ts("RegExpandString"), ts("%TMP%"));
  std::tcout << ts("RegExpandString\t") << reg.read_expand_string(ts("RegExpandString"), ts("")) << std::endl;

  reg.write_float(ts("RegFloat"), 16.09F);
  std::tcout << ts("RegFloat\t") << reg.read_float(ts("RegFloat"), 7.02F) << std::endl;

  reg.write_integer(ts("RegInt"), 1609);
  std::tcout << ts("RegInt\t") << reg.read_integer(ts("RegInt"), 702) << std::endl;

  l.clear();
  l.push_back(ts("String 1"));
  l.push_back(ts("String 2"));
  l.push_back(ts("String 3"));
  reg.write_multi_string(ts("RegMultiString"), l);
  l.clear();
  std::tcout << ts("RegMultiString") << std::endl;
  l = reg.read_multi_string(ts("RegMultiString"), l);
  for (auto e : l) {
    std::tcout << ts("\t") << e << std::endl;
  }

  reg.write_string(ts("RegString"), ts("This is a string"));
  std::tcout << ts("RegString\t") << reg.read_string(ts("RegString"), ts("")) << std::endl;

  if (!reg.close_key())
  {
    std::tcout << ts("Reg -> Close ->Failed") << vu::get_last_error(reg.get_last_error_code()) << std::endl;
  }

  return vu::VU_OK;
}
