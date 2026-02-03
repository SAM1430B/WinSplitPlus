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
#include <cstdlib>
#include <iostream>
#include <assert.h>

#include "include\InjectionInfo.h" 

#if defined(_WIN64)
#pragma comment(linker, "/EXPORT:NativeInjectionEntryPoint=NativeInjectionEntryPoint")
#else
#pragma comment(linker, "/EXPORT:NativeInjectionEntryPoint=_NativeInjectionEntryPoint@4")
#endif

InjectionInfo gInjectionInfo;
HMODULE g_hGameModule = NULL;

// FILTERS FOR IGNORING WINDOWS AND CLASSES
bool IgnoreClassA(LPCSTR lpClassName)
{
    if (lpClassName == NULL || IS_INTRESOURCE(lpClassName)) return false;

    // Custom ignore
    if (gInjectionInfo.customIgnoreClassName[0] != L'\0')
    {
        char ansiCustom[IGNORE_MAX_LENGTH];
        wcstombs_s(nullptr, ansiCustom, IGNORE_MAX_LENGTH, gInjectionInfo.customIgnoreClassName, _TRUNCATE);
        if (strstr(lpClassName, ansiCustom) != NULL) return true;
    }

    if (strstr(lpClassName, "GtoMnK") != NULL) return true;
    if (strstr(lpClassName, "GtoMnK_Pointer_Window") != NULL) return true;
    if (strstr(lpClassName, "GtoMnK_RawInput_Window") != NULL) return true;
    if (strstr(lpClassName, "GtoMnK_Overlay_Window") != NULL) return true;
    
    if (strstr(lpClassName, "ProtoInputPointer") != NULL) return true;
    if (strstr(lpClassName, "PROTORAWINPUT") != NULL) return true;
    if (strstr(lpClassName, "PROTOINPUTUI") != NULL) return true;
    if (strstr(lpClassName, "PROTOHOSTRAWINPUT") != NULL) return true;
    if (strstr(lpClassName, "PROTOHOSTUI") != NULL) return true;
	
	if (strstr(lpClassName, "dummy") != NULL) return true;
	if (strstr(lpClassName, "SK_BackgroundWindow") != NULL) return true;
	if (strstr(lpClassName, "SK_HID_Listener_pid") != NULL) return true;
	if (strstr(lpClassName, "Kiero") != NULL) return true;
	if (strstr(lpClassName, "RTSSWndClass") != NULL) return true;
	if (strstr(lpClassName, "Special K Dummy Window Class") != NULL) return true;
	if (strstr(lpClassName, "EOSOVHDummyWindowClass") != NULL) return true;
	if (strstr(lpClassName, "CurseOverlayTemporaryDirect3D11Window") != NULL) return true;
	if (strstr(lpClassName, "TestDX11WindowClass") != NULL) return true;
	if (strstr(lpClassName, "static") != NULL) return true;
	if (strstr(lpClassName, "SKIV_NotificationIcon") != NULL) return true;
	if (strstr(lpClassName, "InvisibleWindowClassNvPresent") != NULL) return true;
	if (strstr(lpClassName, "TempDirect3D11OverlayWindow") != NULL) return true;
	if (strstr(lpClassName, "TempWindowClass") != NULL) return true;
	if (strstr(lpClassName, "DDrawCompatMessageWindow") != NULL) return true;
    if (strstr(lpClassName, "DDrawCompatPresentationWindow") != NULL) return true;

    // Internal helpers
    if (strstr(lpClassName, "Overlay") != NULL) return true;
    if (strstr(lpClassName, "MSXML") != NULL) return true;
    if (strstr(lpClassName, "DirectInput") != NULL) return true;
    if (strstr(lpClassName, "DirectSound") != NULL) return true;
    if (strstr(lpClassName, "Direct3D") != NULL) return true;
    if (strstr(lpClassName, "DirectDraw") != NULL) return true;

    // System
    if (strstr(lpClassName, "IME") != NULL) return true;
    if (strstr(lpClassName, "MSCTF") != NULL) return true;

    return false;
}

bool IgnoreClassW(LPCWSTR lpClassName)
{
    if (lpClassName == NULL || IS_INTRESOURCE(lpClassName)) return false;

    // Custom ignore
    if (gInjectionInfo.customIgnoreClassName[0] != L'\0')
    {
        if (wcsstr(lpClassName, gInjectionInfo.customIgnoreClassName) != NULL) return true;
    }

    if (wcsstr(lpClassName, L"GtoMnK") != NULL) return true;
	if (wcsstr(lpClassName, L"GtoMnK_Pointer_Window") != NULL) return true;
	if (wcsstr(lpClassName, L"GtoMnK_RawInput_Window") != NULL) return true;
	if (wcsstr(lpClassName, L"GtoMnK_Overlay_Window") != NULL) return true;

    if (wcsstr(lpClassName, L"ProtoInputPointer") != NULL) return true;
    if (wcsstr(lpClassName, L"PROTORAWINPUT") != NULL) return true;
	if (wcsstr(lpClassName, L"PROTOINPUTUI") != NULL) return true;
	if (wcsstr(lpClassName, L"PROTOHOSTRAWINPUT") != NULL) return true;
	if (wcsstr(lpClassName, L"PROTOHOSTUI") != NULL) return true;
	
    if (wcsstr(lpClassName, L"dummy") != NULL) return true;
	if (wcsstr(lpClassName, L"SK_BackgroundWindow") != NULL) return true;
	if (wcsstr(lpClassName, L"SK_HID_Listener_pid") != NULL) return true;
	if (wcsstr(lpClassName, L"Kiero") != NULL) return true;
	if (wcsstr(lpClassName, L"RTSSWndClass") != NULL) return true;
	if (wcsstr(lpClassName, L"Special K Dummy Window Class") != NULL) return true;
	if (wcsstr(lpClassName, L"EOSOVHDummyWindowClass") != NULL) return true;
	if (wcsstr(lpClassName, L"CurseOverlayTemporaryDirect3D11Window") != NULL) return true;
	if (wcsstr(lpClassName, L"TestDX11WindowClass") != NULL) return true;
	if (wcsstr(lpClassName, L"static") != NULL) return true;
	if (wcsstr(lpClassName, L"SKIV_NotificationIcon") != NULL) return true;
	if (wcsstr(lpClassName, L"InvisibleWindowClassNvPresent") != NULL) return true;
	if (wcsstr(lpClassName, L"TempDirect3D11OverlayWindow") != NULL) return true;
	if (wcsstr(lpClassName, L"TempWindowClass") != NULL) return true;
	if (wcsstr(lpClassName, L"DDrawCompatMessageWindow") != NULL) return true;
	if (wcsstr(lpClassName, L"DDrawCompatPresentationWindow") != NULL) return true;

    // Internal helpers
    if (wcsstr(lpClassName, L"Overlay") != NULL) return true;
	if (wcsstr(lpClassName, L"MSXML") != NULL) return true;
	if (wcsstr(lpClassName, L"DirectInput") != NULL) return true;
	if (wcsstr(lpClassName, L"DirectSound") != NULL) return true;
	if (wcsstr(lpClassName, L"Direct3D") != NULL) return true;
	if (wcsstr(lpClassName, L"DirectDraw") != NULL) return true;

	// System
	if (wcsstr(lpClassName, L"IME") != NULL) return true;
	if (wcsstr(lpClassName, L"MSCTF") != NULL) return true;

    return false;
}

bool IgnoreWindow(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName)
{
    // Ignore Tool Windows
    if (dwExStyle & WS_EX_TOOLWINDOW) return true;

    // Custom Window Name
    if (lpWindowName != NULL && gInjectionInfo.customIgnoreWindowName[0] != L'\0')
    {
        char ansiCustom[IGNORE_MAX_LENGTH];
        wcstombs_s(nullptr, ansiCustom, IGNORE_MAX_LENGTH, gInjectionInfo.customIgnoreWindowName, _TRUNCATE);
        if (strstr(lpWindowName, ansiCustom) != NULL) return true;
    }

    // System
    if (!IS_INTRESOURCE(lpClassName) && lpClassName != NULL)
    {
        if (strcmp(lpClassName, "IME") == 0) return true;
        if (strcmp(lpClassName, "Default IME") == 0) return true;
        if (strstr(lpClassName, "MSCTFIME") != NULL) return true;
        if (IgnoreClassA(lpClassName)) return true;
    }
    return false;
}

bool IgnoreWindowW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName)
{
    // Ignore Tool Windows
    if (dwExStyle & WS_EX_TOOLWINDOW) return true;

    // Custom Window Name
    if (lpWindowName != NULL && gInjectionInfo.customIgnoreWindowName[0] != L'\0')
    {
        if (wcsstr(lpWindowName, gInjectionInfo.customIgnoreWindowName) != NULL) return true;
    }

    // System
    if (!IS_INTRESOURCE(lpClassName) && lpClassName != NULL)
    {
        if (wcscmp(lpClassName, L"IME") == 0) return true;
        if (wcscmp(lpClassName, L"Default IME") == 0) return true;
        if (wcsstr(lpClassName, L"MSCTFIME") != NULL) return true;
        if (IgnoreClassW(lpClassName)) return true;
    }
    return false;
}

// WINDOW HOOKS
ATOM WINAPI RegisterClassAHook(
    _In_ const WNDCLASSA* lpWndClass
)
{
    // Ignore Class check
    if (IgnoreClassA(lpWndClass->lpszClassName))
    {
        return RegisterClassA(lpWndClass);
    }

    WNDCLASSA wndClass;
    memcpy_s(&wndClass, sizeof(WNDCLASSA), lpWndClass, sizeof(WNDCLASSA));

    if (gInjectionInfo.windowClassName[0] != L'\0')
    {
        static char ansiName[CLASS_NAME_MAX_LENGTH];
        size_t convertedCount;
        wcstombs_s(&convertedCount, ansiName, CLASS_NAME_MAX_LENGTH, gInjectionInfo.windowClassName, CLASS_NAME_MAX_LENGTH);
        wndClass.lpszClassName = ansiName;
    }

    return RegisterClassA(&wndClass);
}

ATOM WINAPI RegisterClassExAHook(
    _In_ const WNDCLASSEXA* lpwcx
)
{
    // Ignore Class check
    if (IgnoreClassA(lpwcx->lpszClassName))
    {
        return RegisterClassExA(lpwcx);
    }

    WNDCLASSEXA wndClassEx;
    memcpy_s(&wndClassEx, sizeof(WNDCLASSEXA), lpwcx, sizeof(WNDCLASSEXA));

    if (gInjectionInfo.windowClassName[0] != L'\0')
    {
        static char ansiName[CLASS_NAME_MAX_LENGTH];
        size_t convertedCount;
        wcstombs_s(&convertedCount, ansiName, CLASS_NAME_MAX_LENGTH, gInjectionInfo.windowClassName, CLASS_NAME_MAX_LENGTH);
        wndClassEx.lpszClassName = ansiName;
    }

    return RegisterClassExA(&wndClassEx);
}

ATOM WINAPI RegisterClassWHook(
    _In_ const WNDCLASSW* lpWndClass
)
{
    // Ignore Class check
    if (IgnoreClassW(lpWndClass->lpszClassName))
    {
        return RegisterClassW(lpWndClass);
    }

    WNDCLASSW wndClass;
    memcpy_s(&wndClass, sizeof(WNDCLASSW), lpWndClass, sizeof(WNDCLASSW));

    if (gInjectionInfo.windowClassName[0] != L'\0')
    {
        wndClass.lpszClassName = gInjectionInfo.windowClassName;
    }

    return RegisterClassW(&wndClass);
}

ATOM WINAPI RegisterClassExWHook(
    _In_ const WNDCLASSEXW* lpwcx
)
{
    // Ignore Class check
    if (IgnoreClassW(lpwcx->lpszClassName))
    {
        return RegisterClassExW(lpwcx);
    }

    WNDCLASSEXW wndClassEx;
    memcpy_s(&wndClassEx, sizeof(WNDCLASSEXW), lpwcx, sizeof(WNDCLASSEXW));

    if (gInjectionInfo.windowClassName[0] != L'\0')
    {
        wndClassEx.lpszClassName = gInjectionInfo.windowClassName;
    }

    return RegisterClassExW(&wndClassEx);
}

HWND WINAPI FindWindowAHook(LPCSTR lpClassName, LPCSTR lpWindowName)
{
    if (lpClassName != NULL && gInjectionInfo.windowClassName[0] != L'\0')
    {
        char ansiNewClassName[CLASS_NAME_MAX_LENGTH];
        wcstombs_s(nullptr, ansiNewClassName, CLASS_NAME_MAX_LENGTH, gInjectionInfo.windowClassName, _TRUNCATE);

        if (lpWindowName != NULL && gInjectionInfo.windowName[0] != L'\0')
        {
            char ansiNewWindowName[WINDOW_NAME_MAX_LENGTH];
            wcstombs_s(nullptr, ansiNewWindowName, WINDOW_NAME_MAX_LENGTH, gInjectionInfo.windowName, _TRUNCATE);

            return FindWindowA(ansiNewClassName, ansiNewWindowName);
        }
        return FindWindowA(ansiNewClassName, lpWindowName);
    }
    return FindWindowA(lpClassName, lpWindowName);
}

HWND WINAPI FindWindowWHook(LPCWSTR lpClassName, LPCWSTR lpWindowName)
{
    if (lpClassName != NULL && gInjectionInfo.windowClassName[0] != L'\0')
    {
        if (lpWindowName != NULL && gInjectionInfo.windowName[0] != L'\0')
        {
            return FindWindowW(gInjectionInfo.windowClassName, gInjectionInfo.windowName);
        }

        return FindWindowW(gInjectionInfo.windowClassName, lpWindowName);
    }
    return FindWindowW(lpClassName, lpWindowName);
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
	// Ignore window check
    if (IgnoreWindow(dwExStyle, lpClassName, lpWindowName))
    {
        return CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    }

    LPCSTR finalClassName = lpClassName;
    if (gInjectionInfo.windowClassName[0] != L'\0')
    {
        static char ansiClassName[CLASS_NAME_MAX_LENGTH];
        wcstombs_s(nullptr, ansiClassName, CLASS_NAME_MAX_LENGTH, gInjectionInfo.windowClassName, _TRUNCATE);
        finalClassName = ansiClassName;
    }

    LPCSTR finalWindowName = lpWindowName;
    if (gInjectionInfo.windowName[0] != L'\0')
    {
        static char ansiWindowName[WINDOW_NAME_MAX_LENGTH];
        wcstombs_s(nullptr, ansiWindowName, WINDOW_NAME_MAX_LENGTH, gInjectionInfo.windowName, _TRUNCATE);
        finalWindowName = ansiWindowName;
    }

    int finalX = gInjectionInfo.windowPosX;
    int finalY = gInjectionInfo.windowPosY;
    int finalWidth = (gInjectionInfo.windowSizeX != 0) ? gInjectionInfo.windowSizeX : nWidth;
    int finalHeight = (gInjectionInfo.windowSizeY != 0) ? gInjectionInfo.windowSizeY : nHeight;

    return CreateWindowExA(dwExStyle, finalClassName, finalWindowName, dwStyle, finalX, finalY,
        finalWidth, finalHeight, hWndParent, hMenu, hInstance, lpParam);
}

HWND WINAPI CreateWindowExWHook(
    _In_     DWORD     dwExStyle,
    _In_opt_ LPCWSTR  lpClassName,
    _In_opt_ LPCWSTR  lpWindowName,
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
    // Ignore window check
    if (IgnoreWindowW(dwExStyle, lpClassName, lpWindowName))
    {
        return CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    }

    LPCWSTR finalClassName = lpClassName;
    if (gInjectionInfo.windowClassName[0] != L'\0')
    {
        finalClassName = gInjectionInfo.windowClassName;
    }

    LPCWSTR finalWindowName = lpWindowName;
    if (gInjectionInfo.windowName[0] != L'\0')
    {
        finalWindowName = gInjectionInfo.windowName;
    }

    int finalX = gInjectionInfo.windowPosX;
    int finalY = gInjectionInfo.windowPosY;
    int finalWidth = (gInjectionInfo.windowSizeX != 0) ? gInjectionInfo.windowSizeX : nWidth;
    int finalHeight = (gInjectionInfo.windowSizeY != 0) ? gInjectionInfo.windowSizeY : nHeight;

    return CreateWindowExW(dwExStyle, finalClassName, finalWindowName, dwStyle, finalX, finalY,
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
    return SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags | SWP_NOMOVE | SWP_NOSIZE);
}

// MUTEX HOOKS
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

HANDLE WINAPI CreateMutexWHook(
    _In_opt_ LPSECURITY_ATTRIBUTES lpMutexAttributes,
    _In_      BOOL                   bInitialOwner,
    _In_opt_ LPCWSTR                lpName
)
{
    if (!lpName) {
        return CreateMutexW(lpMutexAttributes, bInitialOwner, lpName);
    }

    if (wcscmp(gInjectionInfo.mutexOriginalName, lpName) == 0)
    {
        return CreateMutexW(lpMutexAttributes, bInitialOwner, gInjectionInfo.mutexNewName);
    }

    return CreateMutexW(lpMutexAttributes, bInitialOwner, lpName);
}

// HOOK CREATION FUNCTIONS
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
    if (inRemoteInfo->UserDataSize != sizeof(InjectionInfo)) return;
    memcpy_s(&gInjectionInfo, sizeof(InjectionInfo), inRemoteInfo->UserData, inRemoteInfo->UserDataSize);

    std::wcout << "WinSplitPlusIJ Entered\n";

    bool doAnsi = (gInjectionInfo.injectionFlags & InjectionFlags::HOOK_ANSI) == InjectionFlags::HOOK_ANSI;
    bool doUnicode = (gInjectionInfo.injectionFlags & InjectionFlags::HOOK_UNICODE) == InjectionFlags::HOOK_UNICODE;

    bool doStd = (gInjectionInfo.injectionFlags & InjectionFlags::HOOK_STANDARD) == InjectionFlags::HOOK_STANDARD;
    bool doEx = (gInjectionInfo.injectionFlags & InjectionFlags::HOOK_EXTENDED) == InjectionFlags::HOOK_EXTENDED;

    // Install FindWindow Hooks
    if ((gInjectionInfo.injectionFlags & InjectionFlags::HOOK_FIND_WINDOW) == InjectionFlags::HOOK_FIND_WINDOW)
    {
        if (doAnsi) hookFunction("user32", "FindWindowA", FindWindowAHook);
        if (doUnicode) hookFunction("user32", "FindWindowW", FindWindowWHook);
    }

    // Install RegisterClass Hooks
    if ((gInjectionInfo.injectionFlags & InjectionFlags::HOOK_WND_PROC) == InjectionFlags::HOOK_WND_PROC)
    {
        // ANSI
        if (doAnsi && doStd) hookFunction("user32", "RegisterClassA", RegisterClassAHook);
        if (doAnsi && doEx)  hookFunction("user32", "RegisterClassExA", RegisterClassExAHook);

        // Unicode
        if (doUnicode && doStd) hookFunction("user32", "RegisterClassW", RegisterClassWHook);
        if (doUnicode && doEx)  hookFunction("user32", "RegisterClassExW", RegisterClassExWHook);
    }

	// Install CreateWindow Hooks
    if ((gInjectionInfo.injectionFlags & InjectionFlags::HOOK_CREATE_WINDOW) == InjectionFlags::HOOK_CREATE_WINDOW)
    {
        if (doAnsi) hookFunction("user32", "CreateWindowExA", CreateWindowExAHook);
        if (doUnicode) hookFunction("user32", "CreateWindowExW", CreateWindowExWHook);
    }

	// Install SetWindowPos Hook
    if ((gInjectionInfo.injectionFlags & InjectionFlags::HOOK_SET_WINDOW_POS) == InjectionFlags::HOOK_SET_WINDOW_POS)
    {
        hookFunction("user32", "SetWindowPos", SetWindowPosHook);
    }

	// Install CreateMutex Hooks
    if ((gInjectionInfo.injectionFlags & InjectionFlags::HOOK_CREATE_MUTEX) == InjectionFlags::HOOK_CREATE_MUTEX)
    {
        if (doAnsi) hookFunction("Kernel32", "CreateMutexA", CreateMutexAHook);
        if (doUnicode) hookFunction("Kernel32", "CreateMutexW", CreateMutexWHook);
    }

    RhWakeUpProcess();
}