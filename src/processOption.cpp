#include <windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <psapi.h>
#include "processOption.h"
#include "basicEdit.h"

// 根据进程名获取进程 ID
DWORD GetProcessIDByName(const wchar_t *processName)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // 进程快照
    if (snapshot == INVALID_HANDLE_VALUE)
    {
        return 0;
    }

    PROCESSENTRY32W pEntry = {0};            // PROCESSENTRY32 结构体，用于存储进程信息
    pEntry.dwSize = sizeof(PROCESSENTRY32W); // 不初始化的话下面的 Process32First 函数会失败
    if (!Process32FirstW(snapshot, &pEntry)) // 获取第一个进程
    {
        CloseHandle(snapshot);
        return 0;
    }

    // 遍历进程列表
    do
    {
        wchar_t currentProcessName[MAX_PATH] = {0};       // 存储当前进程名
        wcscpy(currentProcessName, pEntry.szExeFile);     // 将当前进程名拷贝到 currentProcessName
        if (wcscmp(currentProcessName, processName) == 0) // 如果当前进程名和目标进程名相同，返回进程 ID
        {
            CloseHandle(snapshot);
            return pEntry.th32ProcessID;
        }
    } while (Process32NextW(snapshot, &pEntry)); // 获取下一个进程

    CloseHandle(snapshot); // 没有找到目标进程，关闭句柄
    return 0;
}

// 根据进程 ID 和模块名获取基址
LPVOID GetModuleBaseAddress(DWORD ProcessID, LPCWSTR moduleName)
{
    LPVOID lpBaseAddress = NULL;

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID); // 打开进程句柄
    if (hProcess == NULL)
    {
        return lpBaseAddress;
    }
    HMODULE hMods[1024];                                               // 存储模块句柄
    DWORD cbNeeded;                                                    // 存储模块句柄数量
    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) //
    {
        DWORD dwModuleCount = cbNeeded / sizeof(HMODULE); // 模块数量
        for (DWORD i = 0; i < dwModuleCount; i++)
        {
            wchar_t szModuleName[MAX_PATH] = {0};                                                               // 存储模块名
            if (GetModuleFileNameExW(hProcess, hMods[i], szModuleName, sizeof(szModuleName) / sizeof(wchar_t))) // 获取模块名
            {
                if (wcsstr(szModuleName, moduleName) != NULL) // 如果模块名包含目标模块名
                {
                    MODULEINFO moduleInfo = {0};
                    if (GetModuleInformation(hProcess, hMods[i], &moduleInfo, sizeof(moduleInfo))) // 获取模块信息，保存到 moduleInfo
                    {
                        lpBaseAddress = moduleInfo.lpBaseOfDll; // 获取模块基址
                        break;
                    }
                }
            }
        }
        CloseHandle(hProcess); // 关闭进程句柄
    }
    return lpBaseAddress;
}

// 修改进程的代码
BOOL WriteCodeToProcessMemory(HANDLE hProcess, LPVOID lpStartAddress, LPCVOID lpBuffer, SIZE_T nSize)
{
    DWORD dwOldProtect;
    // 取消页保护
    if (!VirtualProtectEx(hProcess, lpStartAddress, nSize, PAGE_EXECUTE_READWRITE, &dwOldProtect))
    {
        return FALSE;
    }
    BOOL bResult = WriteProcessMemory(hProcess, lpStartAddress, lpBuffer, nSize, NULL); // 写入代码
    VirtualProtectEx(hProcess, lpStartAddress, nSize, dwOldProtect, &dwOldProtect);     // 开启页保护
    return bResult;
}

