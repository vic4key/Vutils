#pragma once

#ifdef __cplusplus 
#define DLLAPIC extern "C" __declspec(dllexport)
#else
#define DLLAPIC __declspec(dllexport)
#endif

DLLAPIC LRESULT CALLBACK fnCBTProc(int nCode, WPARAM wParam, LPARAM lParam);
DLLAPIC LRESULT CALLBACK fnMouseProc(int nCode, WPARAM wParam, LPARAM lParam);
DLLAPIC LRESULT CALLBACK fnKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
