#pragma once

#include "Sample.h"

DEF_SAMPLE(InputDialog)
{
  vu::CInputDialog dialog(L"How old are you?");
  if (dialog.do_modal() == IDOK)
  {
    std::wcout << dialog.input().to_string() << std::endl;
  }

  return vu::VU_OK;
}
