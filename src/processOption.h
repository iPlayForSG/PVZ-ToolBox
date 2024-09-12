#ifndef PROCESSOPTION_H
#define PROCESSOPTION_H

#include <windows.h>
#include "basicEdit.h"

DWORD GetProcessIDByName(const wchar_t *processName);

LPVOID GetModuleBaseAddress(DWORD ProcessID, LPCWSTR moduleName);

BOOL WriteCodeToProcessMemory(HANDLE hProcess, LPVOID lpStartAddress, LPCVOID lpBuffer, SIZE_T nSize);

BOOL EditTypeValue(HANDLE hProcess, DWORD BaseAddress, editType type, int value);

int GetTypeValue(HANDLE hProcess, DWORD BaseAddress, editType type);

#endif