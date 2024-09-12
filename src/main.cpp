#include <windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <psapi.h>
#include "processOption.h"
#include "basicEdit.h"
#include "Sunlight.h"
#include "Cardslot.h"

#pragma comment(lib, "psapi.lib")
#pragma execution_character_set("UTF-8")

using namespace std;

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
        printf("2. 卡槽菜单\n");
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
            case 2: // 卡槽菜单
            {
                system("cls");
                printf("1. 植物无冷却\n");
                printf("2. 恢复植物冷却\n");
                printf("3. 修改卡槽的植物（坚果保龄球也可用）\n");
                printf("选择一个操作: [ ]\b\b");
                scanf("%d", &option);
                switch (option)
                {
                    case 1: // 植物无冷却
                    {
                        BOOL flag = CardslotUnCoolDown(hProcess, BaseAddress);
                        if (flag)
                            printf("植物无冷却成功\n");
                        else
                            printf("植物无冷却失败\n");
                        Sleep(500);
                        break;
                    }
                    case 2: // 恢复植物冷却
                    {
                        BOOL flag = CardslotCoolDownRollback(hProcess, BaseAddress);
                        if (flag)
                            printf("恢复植物冷却成功\n");
                        else
                            printf("恢复植物冷却失败\n");
                        Sleep(500);
                        break;
                    }
                    case 3: // 修改卡槽的植物
                    {
                        printf("要修改的卡槽: [ ]\b\b");
                        DWORD slot = 0;
                        scanf("%d", &slot);
                        printf("\n注意, 植物的编号请参照主菜单的植物图鉴, 从左往右从上往下, 编号依次为0, 1, 2...46, 47\n");
                        printf("另外: \n模仿者编号为: 48\n坚果保龄球的爆炸坚果编号为: 49\n坚果保龄球的大坚果编号为: 50\n反向双重射手编号为: 52\n");
                        printf("要修改的植物编号: [  ]\b\b\b");
                        DWORD plant = 0;
                        scanf("%d", &plant);
                        if ((plant < 0 || plant > 50) && plant != 52)
                        {
                            printf("植物编号错误，无法修改\n");
                            Sleep(500);
                            break;
                        }
                        BOOL flag = SetCardslotPlant(hProcess, BaseAddress, slot, plant);
                        if (flag)
                            printf("修改卡槽的植物成功\n");
                        else
                            printf("修改卡槽的植物失败\n");
                        Sleep(500);
                        break;
                    }
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