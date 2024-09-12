#ifndef SUNLIGHT_H
#define SUNLIGHT_H

#include <windows.h>

BOOL LockSunlight(HANDLE hProcess, DWORD BaseAddress, int value);

BOOL UnlockSunlight(HANDLE hProcess, DWORD BaseAddress);

#endif