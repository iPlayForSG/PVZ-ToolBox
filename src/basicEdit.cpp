#include <windows.h>
#include "processOption.h"
#include "basicEdit.h"




// 修改某类值
BOOL EditTypeValue(HANDLE hProcess, DWORD BaseAddress, editType type, int value)
{
    DWORD addr = BaseAddress;
    switch (type)
    {
    case Sunlight:
        addr += 0x331CDC;
        ReadProcessMemory(hProcess, (LPVOID)addr, &addr, sizeof(DWORD), NULL);
        addr += 0x868;
        break;
    default:
        break;
    }
    ReadProcessMemory(hProcess, (LPVOID)addr, &addr, sizeof(DWORD), NULL);
    addr += offsetTable[type];
    return WriteProcessMemory(hProcess, (LPVOID)addr, &value, sizeof(DWORD), 0);
}

// 获取某类值
int GetTypeValue(HANDLE hProcess, DWORD BaseAddress, editType type)
{
    DWORD addr = BaseAddress;
    switch (type)
    {
    case Sunlight:
        addr += 0x331CDC;
        ReadProcessMemory(hProcess, (LPVOID)addr, &addr, sizeof(DWORD), NULL);
        addr += 0x868;
        break;
    default:
        break;
    }
    ReadProcessMemory(hProcess, (LPVOID)addr, &addr, sizeof(DWORD), NULL);
    addr += offsetTable[type];
    int value = 0;
    ReadProcessMemory(hProcess, (LPVOID)addr, &value, sizeof(DWORD), 0);
    return value;
}