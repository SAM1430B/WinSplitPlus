/*
This Project is originally written by @SleepKiller from https://www.swbfgamers.com/index.php?topic=11251.msg112827#msg112827 .
And modified by @SAM1430B from splitscreen.me .
*/


// WinSplitPlusIJ.cpp : Defines the exported functions for the DLL application.

#include <Windows.h>
#include <WinUser.h>
#include <easyhook.h>

#include <fstream>
#include <string>
#include <mutex>

#include "include\InectionInfo.h"

#include <cstdlib>
#include <iostream>
#include <assert.h>

InjectionInfo gInjectionInfo;

//WINDOW HOOKS//
ATOM WINAPI RegisterClassAHook(
    _In_ const WNDCLASSA* lpWndClass
)
{
    WNDCLASSA wndClass;
    memcpy_s(&wndClass, sizeof(WNDCLASSA), lpWndClass, sizeof(WNDCLASSA));

    if (gInjectionInfo.windowClassName[0] != L'\0')
    {
        char ansiName[CLASS_NAME_MAX_LENGTH];
        size_t convertedCount;
        wcstombs_s(&convertedCount, ansiName, CLASS_NAME_MAX_LENGTH, gInjectionInfo.windowClassName, CLASS_NAME_MAX_LENGTH);
        wndClass.lpszClassName = ansiName;
    }

    return RegisterClassA(&wndClass);
}

HWND WINAPI CreateWindowExAHook(
    _In_     DWORD     dwExStyle,
    _In_opt_ LPCSTR   lpClassName,
    _In_opt_ LPCSTR   lpWindowName,
    _In_     DWORD     dwStyle,
    _In_     int       x,
    _In_     int       y,
    _In_     int       nWidth,
    _In_     int       nHeight,
    _In_opt_ HWND      hWndParent,
    _In_opt_ HMENU     hMenu,
    _In_opt_ HINSTANCE hInstance,
    _In_opt_ LPVOID    lpParam
)
{
    LPCSTR finalClassName = lpClassName;
    if (gInjectionInfo.windowClassName[0] != L'\0')
    {
        static char ansiClassName[CLASS_NAME_MAX_LENGTH];
        size_t convertedCount;
        wcstombs_s(&convertedCount, ansiClassName, CLASS_NAME_MAX_LENGTH, gInjectionInfo.windowClassName, CLASS_NAME_MAX_LENGTH);
        finalClassName = ansiClassName;
    }

    LPCSTR finalWindowName = lpWindowName;
    if (gInjectionInfo.windowName[0] != L'\0')
    {
        static char ansiWindowName[WINDOW_NAME_MAX_LENGTH];
        size_t convertedCount;
        wcstombs_s(&convertedCount, ansiWindowName, WINDOW_NAME_MAX_LENGTH, gInjectionInfo.windowName, WINDOW_NAME_MAX_LENGTH);
        finalWindowName = ansiWindowName;
    }

    int finalX = (gInjectionInfo.windowPosX != 0) ? gInjectionInfo.windowPosX : x;
    int finalY = (gInjectionInfo.windowPosY != 0) ? gInjectionInfo.windowPosY : y;
    int finalWidth = (gInjectionInfo.windowSizeX != 0) ? gInjectionInfo.windowSizeX : nWidth;
    int finalHeight = (gInjectionInfo.windowSizeY != 0) ? gInjectionInfo.windowSizeY : nHeight;

    return CreateWindowExA(dwExStyle, finalClassName, finalWindowName, dwStyle, finalX, finalY,
        finalWidth, finalHeight, hWndParent, hMenu, hInstance, lpParam);
}

BOOL WINAPI SetWindowPosHook(
    _In_     HWND hWnd,
    _In_opt_ HWND hWndInsertAfter,
    _In_     int  X,
    _In_     int  Y,
    _In_     int  cx,
    _In_     int  cy,
    _In_     UINT uFlags
)
{
    return TRUE;
}

//MUTEX HOOKS//
HANDLE WINAPI CreateMutexAHook(
    _In_opt_ LPSECURITY_ATTRIBUTES lpMutexAttributes,
    _In_     BOOL                  bInitialOwner,
    _In_opt_ LPCSTR                lpName
)
{
    if (!lpName) {
        return CreateMutexA(lpMutexAttributes, bInitialOwner, lpName);
    }

    char originalAnsiName[MUTEX_NAME_MAX_LENGTH];
    size_t convertedChars = 0;
    wcstombs_s(&convertedChars, originalAnsiName, MUTEX_NAME_MAX_LENGTH, gInjectionInfo.mutexOriginalName, _TRUNCATE);

    if (strcmp(originalAnsiName, lpName) == 0)
    {
        char newAnsiMutexName[MUTEX_NAME_MAX_LENGTH];
        wcstombs_s(&convertedChars, newAnsiMutexName, MUTEX_NAME_MAX_LENGTH, gInjectionInfo.mutexNewName, _TRUNCATE);

        return CreateMutexA(lpMutexAttributes, bInitialOwner, newAnsiMutexName);
    }

    return CreateMutexA(lpMutexAttributes, bInitialOwner, lpName);
}

//Hook Creation Functions//
void hookFunction(const char* module, const char* function, PVOID hookFunction)
{
    HMODULE hModule = GetModuleHandleA(module);

    if (hModule == NULL)
    {
        std::wstring wModule(module, module + strlen(module));
        std::wstring wFunction(function, function + strlen(function));
        std::wcerr << L"Failed to get handle for module: " << wModule << L" while trying to hook " << wFunction << L". Module may not be loaded." << std::endl;
        return;
    }

    HOOK_TRACE_INFO hHook = { NULL };
    NTSTATUS result = LhInstallHook(GetProcAddress(hModule, function), hookFunction, NULL, &hHook);
    if (FAILED(result))
    {
        std::wstring wFunction(function, function + strlen(function));
        std::wcerr << L"Failed to install hook for " << wFunction << L": " << RtlGetLastErrorString() << std::endl;
    }
    else
    {
        std::wstring wFunction(function, function + strlen(function));
        std::wcout << wFunction << L" hook installed." << std::endl;
        ULONG ACLEntries[1] = { 0 };
        LhSetExclusiveACL(ACLEntries, 1, &hHook);
    }
}

extern "C" void __declspec(dllexport) __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo);

void __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo)
{
    if (inRemoteInfo->UserDataSize != sizeof(InjectionInfo))
    {
        std::wcerr << "InjectionInfo invalid" << std::endl;
        return;
    }

    memcpy_s(&gInjectionInfo, sizeof(InjectionInfo), inRemoteInfo->UserData, inRemoteInfo->UserDataSize);

    std::wcout << "SplitInject Entered\n";

    if ((gInjectionInfo.injectionFlags & InjectionFlags::HOOK_WND_PROC) == InjectionFlags::HOOK_WND_PROC)
    {
        hookFunction("user32", "RegisterClassA", RegisterClassAHook);
    }

    if ((gInjectionInfo.injectionFlags & InjectionFlags::HOOK_CREATE_WINDOW) == InjectionFlags::HOOK_CREATE_WINDOW)
    {
        hookFunction("user32", "CreateWindowExA", CreateWindowExAHook);
    }

    if ((gInjectionInfo.injectionFlags & InjectionFlags::HOOK_SET_WINDOW_POS) == InjectionFlags::HOOK_SET_WINDOW_POS)
    {
        hookFunction("user32", "SetWindowPos", SetWindowPosHook);
    }

    if ((gInjectionInfo.injectionFlags & InjectionFlags::HOOK_CREATE_MUTEX) == InjectionFlags::HOOK_CREATE_MUTEX)
    {
        hookFunction("Kernel32", "CreateMutexA", CreateMutexAHook);
    }

    RhWakeUpProcess();
}