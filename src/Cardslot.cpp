#include <windows.h>
#include "processOption.h"


BOOL CardslotUnCoolDown(HANDLE hProcess, DWORD BaseAddress)
{
    unsigned char newCode[2] = {0x90, 0x90};
    return WriteCodeToProcessMemory(hProcess, (LPVOID)(BaseAddress + 0x958C5), newCode, 2);
}

BOOL CardslotCoolDownRollback(HANDLE hProcess, DWORD BaseAddress)
{
    unsigned char origCode[2] = {0x7E, 0x14};
    return WriteCodeToProcessMemory(hProcess, (LPVOID)(BaseAddress + 0x958C5), origCode, 2);
}

BOOL SetCardslotPlant(HANDLE hProcess, DWORD BaseAddress, DWORD slot, DWORD plant)
{
    DWORD cardslotAddress = BaseAddress + 0x331CDC;
    ReadProcessMemory(hProcess, (LPVOID)cardslotAddress, &cardslotAddress, sizeof(DWORD), NULL);
    cardslotAddress += 0x868;
    ReadProcessMemory(hProcess, (LPVOID)cardslotAddress, &cardslotAddress, sizeof(DWORD), NULL);
    cardslotAddress += 0x15C;
    ReadProcessMemory(hProcess, (LPVOID)cardslotAddress, &cardslotAddress, sizeof(DWORD), NULL);
    cardslotAddress += 0x5c;
    cardslotAddress += (slot - 1) * 0x50;
    return WriteProcessMemory(hProcess, (LPVOID)cardslotAddress, &plant, sizeof(DWORD), NULL);
}