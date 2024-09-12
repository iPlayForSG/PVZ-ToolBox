#include <windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <psapi.h>

#pragma comment(lib, "psapi.lib")
#pragma execution_character_set("UTF-8")

using namespace std;

enum editType
{
    Sunlight
};
unsigned int offsetTable[] = {0x5578};

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
        wchar_t currentProcessName[MAX_PATH] = {0}; // 存储当前进程名
        wcscpy(currentProcessName, pEntry.szExeFile); // 将当前进程名拷贝到 currentProcessName
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
    BOOL bResult = WriteProcessMemory(hProcess, lpStartAddress, lpBuffer, nSize, NULL);  // 写入代码
    VirtualProtectEx(hProcess, lpStartAddress, nSize, dwOldProtect, &dwOldProtect);      // 开启页保护
    return bResult;
}

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

// 主菜单，后续换成 imgui，现在命令行将就着用用吧
void CheatMenu(HANDLE hProcess, DWORD pid, DWORD BaseAddress)
{
    cout << "pid: " << pid << endl;
    cout << "BaseAddress: " << hex << BaseAddress << endl;
    cout << "DEBUGGING: " << dec << GetTypeValue(hProcess, BaseAddress, Sunlight) << endl;
    DWORD option = 0;
    unsigned int value = 0;
    while (true)
    {
        system("cls");
        printf("PVZ Tools\n");
        printf("=============================================\n");
        printf("0. 退出工具箱\n");
        printf("1. 阳光菜单\n");
        printf("=============================================\n");
        printf("选择一个操作: [ ]\b\b");
        scanf("%d", &option);
        switch (option)
        {
            system("cls");
            case 0:
                return;
            case 1: // 阳光菜单
            {
                system("cls");
                printf("1. 修改阳光\n");
                printf("2. 锁定阳光\n");
                printf("3. 解除阳光锁定\n");
                printf("选择一个操作: [ ]\b\b");
                scanf("%d", &option);
                switch (option)
                {
                    case 1: // 修改阳光
                    {
                        int currentSunlight = GetTypeValue(hProcess, BaseAddress, Sunlight);
                        printf("当前阳光为: %d\n将其修改为: ", currentSunlight);
                        int aimSunlight = 0;
                        scanf("%d", &aimSunlight);
                        BOOL flag = EditTypeValue(hProcess, BaseAddress, Sunlight, aimSunlight);
                        if (flag)
                            printf("修改阳光成功\n");
                        else
                            printf("修改阳光失败\n");
                        Sleep(500);
                        break;
                    }
                    case 2: // 锁定阳光
                    {
                        printf("锁定阳光为: \n");
                        int lockSunlight = 0;
                        scanf("%d", &lockSunlight);
                        BOOL flag = LockSunlight(hProcess, BaseAddress, lockSunlight);
                        if (flag)
                            printf("锁定阳光成功\n");
                        else
                            printf("锁定阳光失败\n");
                        Sleep(500);
                        break;
                    }
                    case 3: // 解除阳光锁定
                    {
                        BOOL flag = UnlockSunlight(hProcess, BaseAddress);
                        if (flag)
                            printf("解除阳光锁定成功\n");
                        else
                            printf("解除阳光锁定失败\n");
                        Sleep(500);
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
        }
    }
}

int main()
{
    DWORD pid = GetProcessIDByName(L"PlantsVsZombies.exe");
    if (pid == 0)
    {
        cout << "GetProcessIDByName failed" << endl;
        return 0;
    }
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    uintptr_t BaseAddress = reinterpret_cast<uintptr_t>(GetModuleBaseAddress(pid, L"PlantsVsZombies.exe"));
    if (BaseAddress == NULL)
    {
        cout << "GetModuleBaseAddress failed" << endl;
        return 0;
    }
    CheatMenu(hProcess, pid, BaseAddress);
    return 0;
}