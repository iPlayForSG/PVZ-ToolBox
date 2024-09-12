#ifndef CARDSLOT_H
#define CARDSLOT_H

#include <windows.h>

BOOL CardslotUnCoolDown(HANDLE hProcess, DWORD BaseAddress);

BOOL CardslotCoolDownRollback(HANDLE hProcess, DWORD BaseAddress);

BOOL SetCardslotPlant(HANDLE hProcess, DWORD BaseAddress, DWORD slot, DWORD plant);

#endif