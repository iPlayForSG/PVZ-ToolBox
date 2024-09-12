#include <windows.h>
#include "processOption.h"
#include "basicEdit.h"
// 锁定阳光
BOOL LockSunlight(HANDLE hProcess, DWORD BaseAddress, int value)
{
    unsigned char newCode_addSunlight[6] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
    unsigned char newCode_subSunlight[2] = {0x90, 0x90};
    BOOL flag = 1;
    flag &= EditTypeValue(hProcess, BaseAddress, Sunlight, value);
    flag &= WriteCodeToProcessMemory(hProcess, (LPVOID)(BaseAddress + 0x1F4D0), newCode_addSunlight, 6);
    flag &= WriteCodeToProcessMemory(hProcess, (LPVOID)(BaseAddress + 0x1F634), newCode_subSunlight, 2);
    return flag;
}

// 解除阳光锁定
BOOL UnlockSunlight(HANDLE hProcess, DWORD BaseAddress)
{
    unsigned char origCode_addSunlight[6] = {0x01, 0x88, 0x78, 0x55, 0x00, 0x00};
    unsigned char origCode_subSunlight[2] = {0x2B, 0xF3};
    BOOL flag = 1;
    flag &= WriteCodeToProcessMemory(hProcess, (LPVOID)(BaseAddress + 0x1F4D0), origCode_addSunlight, 6);
    flag &= WriteCodeToProcessMemory(hProcess, (LPVOID)(BaseAddress + 0x1F634), origCode_subSunlight, 2);
    return flag;
}