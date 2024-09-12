#ifndef BASICEDIT_H
#define BASICEDIT_H

#include <windows.h>

enum editType
{
    Sunlight
};

const unsigned int offsetTable[] = { 0x5578 };

BOOL EditTypeValue(HANDLE hProcess, DWORD BaseAddress, editType type, int value);

int GetTypeValue(HANDLE hProcess, DWORD BaseAddress, editType type);

#endif