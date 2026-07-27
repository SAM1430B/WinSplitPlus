/*
This Project is originally written by @SleepKiller from https://www.swbfgamers.com/index.php?topic=11251.msg112827#msg112827 .
And modified by @SAM1430B from splitscreen.me .
*/

// WinSplitPlusIJ.cpp : Defines the exported functions for the DLL application.

#include <Windows.h>
#include <WinUser.h>
#include <easyhook.h>

#include <string>
#include <iostream>

#include "include\InjectionInfo.h" 

#define LOG_PREFIX "[DLL]"
#define LOG_PREFIX_W L"[DLL]"
#include "include\Logger.h"

#include "WindowFilters.h"

#if defined(_WIN64)
#pragma comment(linker, "/EXPORT:NativeInjectionEntryPoint=NativeInjectionEntryPoint")
#else
#pragma comment(linker, "/EXPORT:NativeInjectionEntryPoint=_NativeInjectionEntryPoint@4")
#endif

InjectionInfo gInjectionInfo;
bool g_LoggingEnabled = false;
HMODULE g_hGameModule = NULL;

char g_SpoofedClassNameA[CLASS_NAME_MAX_LENGTH] = { 0 };
wchar_t g_SpoofedClassNameW[CLASS_NAME_MAX_LENGTH] = { 0 };

// WINDOW HOOKS
ATOM WINAPI RegisterClassAHook(_In_ const WNDCLASSA* lpWndClass)
{
    DEBUG_LOG_W(L"RegisterClassAHook called.");
    // Filter
    if (IsCallerIgnoredModule() || IgnoreClassA(lpWndClass->lpszClassName)) return RegisterClassA(lpWndClass);

    WNDCLASSA wndClass;
    memcpy_s(&wndClass, sizeof(WNDCLASSA), lpWndClass, sizeof(WNDCLASSA));

    if (gInjectionInfo.windowClassName[0] != L'\0' && lpWndClass->lpszClassName != NULL && !IS_INTRESOURCE(lpWndClass->lpszClassName))
    {
        if (g_SpoofedClassNameA[0] == '\0') {
            strcpy_s(g_SpoofedClassNameA, CLASS_NAME_MAX_LENGTH, lpWndClass->lpszClassName);

        char ansiName[CLASS_NAME_MAX_LENGTH];
        size_t convertedCount;
            wcstombs_s(&convertedCount, ansiName, CLASS_NAME_MAX_LENGTH, gInjectionInfo.windowClassName, _TRUNCATE);
        wndClass.lpszClassName = ansiName;
    }
        else {
            return RegisterClassA(lpWndClass);
        }
    }
    return RegisterClassA(&wndClass);
}

ATOM WINAPI RegisterClassExAHook(_In_ const WNDCLASSEXA* lpwcx)
{
    DEBUG_LOG_W(L"RegisterClassExAHook called.");
    // Filter
	if (IsCallerIgnoredModule() || IgnoreClassA(lpwcx->lpszClassName)) return RegisterClassExA(lpwcx);

    WNDCLASSEXA wndClassEx;
    memcpy_s(&wndClassEx, sizeof(WNDCLASSEXA), lpwcx, sizeof(WNDCLASSEXA));

    if (gInjectionInfo.windowClassName[0] != L'\0' && lpwcx->lpszClassName != NULL && !IS_INTRESOURCE(lpwcx->lpszClassName))
    {
        if (g_SpoofedClassNameA[0] == '\0') {
            strcpy_s(g_SpoofedClassNameA, CLASS_NAME_MAX_LENGTH, lpwcx->lpszClassName);

        char ansiName[CLASS_NAME_MAX_LENGTH];
        size_t convertedCount;
            wcstombs_s(&convertedCount, ansiName, CLASS_NAME_MAX_LENGTH, gInjectionInfo.windowClassName, _TRUNCATE);
        wndClassEx.lpszClassName = ansiName;
    }
        else {
            return RegisterClassExA(lpwcx);
        }
    }
    return RegisterClassExA(&wndClassEx);
}

ATOM WINAPI RegisterClassWHook(_In_ const WNDCLASSW* lpWndClass)
{
    DEBUG_LOG_W(L"RegisterClassWHook called.");
    // Filter
	if (IsCallerIgnoredModule() || IgnoreClassW(lpWndClass->lpszClassName)) return RegisterClassW(lpWndClass);

    WNDCLASSW wndClass;
    memcpy_s(&wndClass, sizeof(WNDCLASSW), lpWndClass, sizeof(WNDCLASSW));

    if (gInjectionInfo.windowClassName[0] != L'\0' && lpWndClass->lpszClassName != NULL && !IS_INTRESOURCE(lpWndClass->lpszClassName))
    {
        if (g_SpoofedClassNameW[0] == L'\0') {
            wcscpy_s(g_SpoofedClassNameW, CLASS_NAME_MAX_LENGTH, lpWndClass->lpszClassName);
        wndClass.lpszClassName = gInjectionInfo.windowClassName;
    }
        else {
            return RegisterClassW(lpWndClass);
        }
    }
    return RegisterClassW(&wndClass);
}

ATOM WINAPI RegisterClassExWHook(_In_ const WNDCLASSEXW* lpwcx)
{
    DEBUG_LOG_W(L"RegisterClassExWHook called.");
    // Filter
	if (IsCallerIgnoredModule() || IgnoreClassW(lpwcx->lpszClassName)) return RegisterClassExW(lpwcx);

    WNDCLASSEXW wndClassEx;
    memcpy_s(&wndClassEx, sizeof(WNDCLASSEXW), lpwcx, sizeof(WNDCLASSEXW));

    if (gInjectionInfo.windowClassName[0] != L'\0' && lpwcx->lpszClassName != NULL && !IS_INTRESOURCE(lpwcx->lpszClassName))
    {
        if (g_SpoofedClassNameW[0] == L'\0') {
            wcscpy_s(g_SpoofedClassNameW, CLASS_NAME_MAX_LENGTH, lpwcx->lpszClassName);
        wndClassEx.lpszClassName = gInjectionInfo.windowClassName;
    }
        else {
            return RegisterClassExW(lpwcx);
        }
    }
    return RegisterClassExW(&wndClassEx);
}

HWND WINAPI FindWindowAHook(LPCSTR lpClassName, LPCSTR lpWindowName)
{
    DEBUG_LOG_W(L"FindWindowAHook called.");
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
    DEBUG_LOG_W(L"FindWindowWHook called.");
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

HWND WINAPI FindWindowExAHook(HWND hWndParent, HWND hWndChildAfter, LPCSTR lpszClass, LPCSTR lpszWindow)
{
    DEBUG_LOG_W(L"FindWindowExAHook called.");
    if (lpszClass != NULL && gInjectionInfo.windowClassName[0] != L'\0')
    {
        char ansiNewClassName[CLASS_NAME_MAX_LENGTH];
        wcstombs_s(nullptr, ansiNewClassName, CLASS_NAME_MAX_LENGTH, gInjectionInfo.windowClassName, _TRUNCATE);

        if (lpszWindow != NULL && gInjectionInfo.windowName[0] != L'\0')
        {
            char ansiNewWindowName[WINDOW_NAME_MAX_LENGTH];
            wcstombs_s(nullptr, ansiNewWindowName, WINDOW_NAME_MAX_LENGTH, gInjectionInfo.windowName, _TRUNCATE);
            return FindWindowExA(hWndParent, hWndChildAfter, ansiNewClassName, ansiNewWindowName);
        }
        return FindWindowExA(hWndParent, hWndChildAfter, ansiNewClassName, lpszWindow);
    }
    return FindWindowExA(hWndParent, hWndChildAfter, lpszClass, lpszWindow);
}

HWND WINAPI FindWindowExWHook(HWND hWndParent, HWND hWndChildAfter, LPCWSTR lpszClass, LPCWSTR lpszWindow)
{
    DEBUG_LOG_W(L"FindWindowExWHook called.");
    if (lpszClass != NULL && gInjectionInfo.windowClassName[0] != L'\0')
    {
        if (lpszWindow != NULL && gInjectionInfo.windowName[0] != L'\0')
        {
            return FindWindowExW(hWndParent, hWndChildAfter, gInjectionInfo.windowClassName, gInjectionInfo.windowName);
        }
        return FindWindowExW(hWndParent, hWndChildAfter, gInjectionInfo.windowClassName, lpszWindow);
    }
    return FindWindowExW(hWndParent, hWndChildAfter, lpszClass, lpszWindow);
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
    DEBUG_LOG_W(L"CreateWindowExAHook called.");
    if (lpClassName && !IS_INTRESOURCE(lpClassName)) {
        DEBUG_LOG_W(L"Requested ClassName (A): " << lpClassName);
    }
    if (lpWindowName) {
        DEBUG_LOG_W(L"Requested WindowName (A): " << lpWindowName);
    }

	// Filter
    if (IsCallerIgnoredModule()) {
        DEBUG_LOG_W(L"Ignoring CreateWindowExA due to IsCallerIgnoredModule.");
        return CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    }
    if (IgnoreClassA(lpClassName)) {
        DEBUG_LOG_W(L"Ignoring CreateWindowExA due to IgnoreClassA.");
        return CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    }

    if (dwStyle & WS_CHILD) {
        return CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    }

    LPCSTR finalClassName = lpClassName;
    std::string spoofedClassName;

    if (gInjectionInfo.windowClassName[0] != L'\0' && lpClassName != NULL && !IS_INTRESOURCE(lpClassName))
    {
        if (strcmp(lpClassName, g_SpoofedClassNameA) == 0)
        {
        char ansiClassName[CLASS_NAME_MAX_LENGTH];
        wcstombs_s(nullptr, ansiClassName, CLASS_NAME_MAX_LENGTH, gInjectionInfo.windowClassName, _TRUNCATE);
            spoofedClassName = ansiClassName;
            finalClassName = spoofedClassName.c_str();
    }
    }

    LPCSTR finalWindowName = lpWindowName;
    std::string spoofedWindowName;

    if (gInjectionInfo.windowName[0] != L'\0')
    {
        char ansiWindowName[WINDOW_NAME_MAX_LENGTH];
        wcstombs_s(nullptr, ansiWindowName, WINDOW_NAME_MAX_LENGTH, gInjectionInfo.windowName, _TRUNCATE);
        spoofedWindowName = ansiWindowName;
        finalWindowName = spoofedWindowName.c_str();
    }

    int finalX = (gInjectionInfo.windowPosX != CW_USEDEFAULT) ? gInjectionInfo.windowPosX : x;
    int finalY = (gInjectionInfo.windowPosY != CW_USEDEFAULT) ? gInjectionInfo.windowPosY : y;
    int finalWidth = (gInjectionInfo.windowSizeX != 0) ? gInjectionInfo.windowSizeX : nWidth;
    int finalHeight = (gInjectionInfo.windowSizeY != 0) ? gInjectionInfo.windowSizeY : nHeight;

    if ((gInjectionInfo.injectionFlags & InjectionFlags::HOOK_FORCE_WINDOW) == InjectionFlags::HOOK_FORCE_WINDOW) {
        dwStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
        dwStyle |= WS_POPUP;
        dwExStyle &= ~(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE);
    }

    DEBUG_LOG_W(L"Applying CreateWindowExA with Size: " << finalWidth << L"x" << finalHeight << L" Pos: " << finalX << L"," << finalY);

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
    DEBUG_LOG_W(L"CreateWindowExWHook called.");
    if (lpClassName && !IS_INTRESOURCE(lpClassName)) {
        DEBUG_LOG_W(L"Requested ClassName (W): " << lpClassName);
    }
    if (lpWindowName) {
        DEBUG_LOG_W(L"Requested WindowName (W): " << lpWindowName);
    }

    // Filter
    if (IsCallerIgnoredModule()) {
        DEBUG_LOG_W(L"Ignoring CreateWindowExW due to IsCallerIgnoredModule.");
        return CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    }
    if (IgnoreClassW(lpClassName)) {
        DEBUG_LOG_W(L"Ignoring CreateWindowExW due to IgnoreClassW.");
        return CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    }

    if (dwStyle & WS_CHILD) {
        return CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    }

    LPCWSTR finalClassName = lpClassName;
    if (gInjectionInfo.windowClassName[0] != L'\0' && lpClassName != NULL && !IS_INTRESOURCE(lpClassName))
    {
        if (wcscmp(lpClassName, g_SpoofedClassNameW) == 0)
        {
        finalClassName = gInjectionInfo.windowClassName;
    }
    }

    LPCWSTR finalWindowName = lpWindowName;
    if (gInjectionInfo.windowName[0] != L'\0')
    {
        finalWindowName = gInjectionInfo.windowName;
    }

    int finalX = (gInjectionInfo.windowPosX != CW_USEDEFAULT) ? gInjectionInfo.windowPosX : x;
    int finalY = (gInjectionInfo.windowPosY != CW_USEDEFAULT) ? gInjectionInfo.windowPosY : y;
    int finalWidth = (gInjectionInfo.windowSizeX != 0) ? gInjectionInfo.windowSizeX : nWidth;
    int finalHeight = (gInjectionInfo.windowSizeY != 0) ? gInjectionInfo.windowSizeY : nHeight;

    if ((gInjectionInfo.injectionFlags & InjectionFlags::HOOK_FORCE_WINDOW) == InjectionFlags::HOOK_FORCE_WINDOW) {
        dwStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
        dwStyle |= WS_POPUP;
        dwExStyle &= ~(WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE);
    }

    DEBUG_LOG_W(L"Applying CreateWindowExW with Size: " << finalWidth << L"x" << finalHeight << L" Pos: " << finalX << L"," << finalY);

    return CreateWindowExW(dwExStyle, finalClassName, finalWindowName, dwStyle, finalX, finalY,
        finalWidth, finalHeight, hWndParent, hMenu, hInstance, lpParam);
}

HWND WINAPI CreateWindowAHook(LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    DEBUG_LOG_W(L"CreateWindowAHook called.");
    return CreateWindowExAHook(0, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

HWND WINAPI CreateWindowWHook(LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    DEBUG_LOG_W(L"CreateWindowWHook called.");
    return CreateWindowExWHook(0, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
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
    DEBUG_LOG_W(L"SetWindowPosHook called.");
    int finalX = (gInjectionInfo.windowPosX != CW_USEDEFAULT) ? gInjectionInfo.windowPosX : X;
    int finalY = (gInjectionInfo.windowPosY != CW_USEDEFAULT) ? gInjectionInfo.windowPosY : Y;
    int finalCx = (gInjectionInfo.windowSizeX != 0) ? gInjectionInfo.windowSizeX : cx;
    int finalCy = (gInjectionInfo.windowSizeY != 0) ? gInjectionInfo.windowSizeY : cy;

    if (gInjectionInfo.windowPosX != CW_USEDEFAULT || gInjectionInfo.windowPosY != CW_USEDEFAULT) {
        uFlags &= ~SWP_NOMOVE;
    }

    if (gInjectionInfo.windowSizeX != 0 || gInjectionInfo.windowSizeY != 0) {
        uFlags &= ~SWP_NOSIZE;
    }

    return SetWindowPos(hWnd, hWndInsertAfter, finalX, finalY, finalCx, finalCy, uFlags);
}

BOOL WINAPI MoveWindowHook(HWND hWnd, int X, int Y, int nWidth, int nHeight, BOOL bRepaint)
{
    DEBUG_LOG_W(L"MoveWindowHook called.");
    int finalX = (gInjectionInfo.windowPosX != CW_USEDEFAULT) ? gInjectionInfo.windowPosX : X;
    int finalY = (gInjectionInfo.windowPosY != CW_USEDEFAULT) ? gInjectionInfo.windowPosY : Y;
    int finalWidth = (gInjectionInfo.windowSizeX != 0) ? gInjectionInfo.windowSizeX : nWidth;
    int finalHeight = (gInjectionInfo.windowSizeY != 0) ? gInjectionInfo.windowSizeY : nHeight;

    return MoveWindow(hWnd, finalX, finalY, finalWidth, finalHeight, bRepaint);
}

// DISPLAY HOOKS
BOOL WINAPI AdjustWindowRectHook(LPRECT lpRect, DWORD dwStyle, BOOL bMenu)
{
    DEBUG_LOG_W(L"AdjustWindowRectHook called.");
    return TRUE;
}

BOOL WINAPI AdjustWindowRectExHook(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle)
{
    DEBUG_LOG_W(L"AdjustWindowRectExHook called.");
    return TRUE;
}

int WINAPI GetSystemMetricsHook(int nIndex)
{
    DEBUG_LOG_W(L"GetSystemMetricsHook called.");
    // Filter
    if (IsCallerIgnoredModule()) return GetSystemMetrics(nIndex);

    if (nIndex == SM_CXSCREEN && gInjectionInfo.windowSizeX != 0) return gInjectionInfo.windowSizeX;
    if (nIndex == SM_CYSCREEN && gInjectionInfo.windowSizeY != 0) return gInjectionInfo.windowSizeY;
    return GetSystemMetrics(nIndex);
}

LONG WINAPI ChangeDisplaySettingsAHook(DEVMODEA* lpDevMode, DWORD dwFlags) {
    DEBUG_LOG_W(L"ChangeDisplaySettingsAHook called."); return DISP_CHANGE_SUCCESSFUL; }
LONG WINAPI ChangeDisplaySettingsWHook(DEVMODEW* lpDevMode, DWORD dwFlags) {
    DEBUG_LOG_W(L"ChangeDisplaySettingsWHook called."); return DISP_CHANGE_SUCCESSFUL; }
LONG WINAPI ChangeDisplaySettingsExAHook(LPCSTR lpszDeviceName, DEVMODEA* lpDevMode, HWND hwnd, DWORD dwflags, LPVOID lParam) {
    DEBUG_LOG_W(L"ChangeDisplaySettingsExAHook called."); return DISP_CHANGE_SUCCESSFUL; }
LONG WINAPI ChangeDisplaySettingsExWHook(LPCWSTR lpszDeviceName, DEVMODEW* lpDevMode, HWND hwnd, DWORD dwflags, LPVOID lParam) {
    DEBUG_LOG_W(L"ChangeDisplaySettingsExWHook called."); return DISP_CHANGE_SUCCESSFUL; }

LONG_PTR WINAPI SetWindowLongPtrAHook(HWND hWnd, int nIndex, LONG_PTR dwNewLong)
{
    DEBUG_LOG_W(L"SetWindowLongPtrAHook called.");
    // Filter
    if (nIndex == GWL_STYLE && !IsIgnoredHwnd(hWnd) && !IsCallerIgnoredModule()) {
        dwNewLong &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
        dwNewLong |= WS_POPUP;
    }
    return SetWindowLongPtrA(hWnd, nIndex, dwNewLong);
}

LONG_PTR WINAPI SetWindowLongPtrWHook(HWND hWnd, int nIndex, LONG_PTR dwNewLong)
{
    DEBUG_LOG_W(L"SetWindowLongPtrWHook called.");
    // Filter
    if (nIndex == GWL_STYLE && !IsIgnoredHwnd(hWnd) && !IsCallerIgnoredModule()) {
        dwNewLong &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
        dwNewLong |= WS_POPUP;
    }
    return SetWindowLongPtrW(hWnd, nIndex, dwNewLong);
}

BOOL WINAPI GetMonitorInfoAHook(HMONITOR hMonitor, LPMONITORINFO lpmi)
{
    DEBUG_LOG_W(L"GetMonitorInfoAHook called.");
    BOOL result = GetMonitorInfoA(hMonitor, lpmi);

	// Filter
    if (IsCallerIgnoredModule()) return result;

    if (result && lpmi != NULL) {
        if (gInjectionInfo.windowPosX != CW_USEDEFAULT) {lpmi->rcMonitor.left = gInjectionInfo.windowPosX;}
        if (gInjectionInfo.windowPosY != CW_USEDEFAULT) {lpmi->rcMonitor.top = gInjectionInfo.windowPosY;}

        if (gInjectionInfo.windowSizeX != 0) {lpmi->rcMonitor.right = lpmi->rcMonitor.left + gInjectionInfo.windowSizeX;}
        if (gInjectionInfo.windowSizeY != 0) {lpmi->rcMonitor.bottom = lpmi->rcMonitor.top + gInjectionInfo.windowSizeY;}

        lpmi->rcWork = lpmi->rcMonitor;
    }
    return result;
}

BOOL WINAPI GetMonitorInfoWHook(HMONITOR hMonitor, LPMONITORINFO lpmi)
{
    DEBUG_LOG_W(L"GetMonitorInfoWHook called.");
    BOOL result = GetMonitorInfoW(hMonitor, lpmi);

	// Filter
	if (IsCallerIgnoredModule()) return result;

    if (result && lpmi != NULL) {
        if (gInjectionInfo.windowPosX != CW_USEDEFAULT) lpmi->rcMonitor.left = gInjectionInfo.windowPosX;
        if (gInjectionInfo.windowPosY != CW_USEDEFAULT) lpmi->rcMonitor.top = gInjectionInfo.windowPosY;

        if (gInjectionInfo.windowSizeX != 0) lpmi->rcMonitor.right = lpmi->rcMonitor.left + gInjectionInfo.windowSizeX;
        if (gInjectionInfo.windowSizeY != 0) lpmi->rcMonitor.bottom = lpmi->rcMonitor.top + gInjectionInfo.windowSizeY;

        lpmi->rcWork = lpmi->rcMonitor;
    }
    return result;
}

BOOL WINAPI SystemParametersInfoAHook(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni)
{
    DEBUG_LOG_W(L"SystemParametersInfoAHook called.");
    BOOL result = SystemParametersInfoA(uiAction, uiParam, pvParam, fWinIni);

	// Filter
	if (IsCallerIgnoredModule()) return result;

    if (uiAction == SPI_GETWORKAREA && pvParam != NULL && result) {
        LPRECT rect = (LPRECT)pvParam;

        if (gInjectionInfo.windowPosX != CW_USEDEFAULT) rect->left = gInjectionInfo.windowPosX;
        if (gInjectionInfo.windowPosY != CW_USEDEFAULT) rect->top = gInjectionInfo.windowPosY;

        if (gInjectionInfo.windowSizeX != 0) rect->right = rect->left + gInjectionInfo.windowSizeX;
        if (gInjectionInfo.windowSizeY != 0) rect->bottom = rect->top + gInjectionInfo.windowSizeY;
    }
    return result;
}

BOOL WINAPI SystemParametersInfoWHook(UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni)
{
    DEBUG_LOG_W(L"SystemParametersInfoWHook called.");
    BOOL result = SystemParametersInfoW(uiAction, uiParam, pvParam, fWinIni);

	// Filter
	if (IsCallerIgnoredModule()) return result;

    if (uiAction == SPI_GETWORKAREA && pvParam != NULL && result) {
        LPRECT rect = (LPRECT)pvParam;

        if (gInjectionInfo.windowPosX != CW_USEDEFAULT) rect->left = gInjectionInfo.windowPosX;
        if (gInjectionInfo.windowPosY != CW_USEDEFAULT) rect->top = gInjectionInfo.windowPosY;

        if (gInjectionInfo.windowSizeX != 0) rect->right = rect->left + gInjectionInfo.windowSizeX;
        if (gInjectionInfo.windowSizeY != 0) rect->bottom = rect->top + gInjectionInfo.windowSizeY;
    }
    return result;
}

BOOL WINAPI EnumDisplaySettingsAHook(LPCSTR lpszDeviceName, DWORD iModeNum, DEVMODEA* lpDevMode)
{
    DEBUG_LOG_W(L"EnumDisplaySettingsAHook called.");
    BOOL result = EnumDisplaySettingsA(lpszDeviceName, iModeNum, lpDevMode);

	// Filter
    if (IsCallerIgnoredModule()) return result;

    if (result && lpDevMode != NULL && gInjectionInfo.windowSizeX != 0) {
        if (lpDevMode->dmPelsWidth < (DWORD)gInjectionInfo.windowSizeX) lpDevMode->dmPelsWidth = gInjectionInfo.windowSizeX;
        if (lpDevMode->dmPelsHeight < (DWORD)gInjectionInfo.windowSizeY) lpDevMode->dmPelsHeight = gInjectionInfo.windowSizeY;
    }
    return result;
}

BOOL WINAPI EnumDisplaySettingsWHook(LPCWSTR lpszDeviceName, DWORD iModeNum, DEVMODEW* lpDevMode)
{
    DEBUG_LOG_W(L"EnumDisplaySettingsWHook called.");
    BOOL result = EnumDisplaySettingsW(lpszDeviceName, iModeNum, lpDevMode);

	// Filter
	if (IsCallerIgnoredModule()) return result;

    if (result && lpDevMode != NULL && gInjectionInfo.windowSizeX != 0) {
        if (lpDevMode->dmPelsWidth < (DWORD)gInjectionInfo.windowSizeX) lpDevMode->dmPelsWidth = gInjectionInfo.windowSizeX;
        if (lpDevMode->dmPelsHeight < (DWORD)gInjectionInfo.windowSizeY) lpDevMode->dmPelsHeight = gInjectionInfo.windowSizeY;
}
    return result;
}

// MUTEX HOOKS
HANDLE WINAPI CreateMutexAHook(
    _In_opt_ LPSECURITY_ATTRIBUTES lpMutexAttributes,
    _In_     BOOL                  bInitialOwner,
    _In_opt_ LPCSTR                lpName
)
{
    DEBUG_LOG_W(L"CreateMutexAHook called.");
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
    DEBUG_LOG_W(L"CreateMutexWHook called.");
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
        DEBUG_LOG_W(L"Failed to get handle for module: " << wModule << L" while trying to hook " << wFunction << L". Module may not be loaded.");
        return;
    }

    HOOK_TRACE_INFO hHook = { NULL };
    NTSTATUS result = LhInstallHook(GetProcAddress(hModule, function), hookFunction, NULL, &hHook);
    if (FAILED(result))
    {
        std::wstring wFunction(function, function + strlen(function));
        DEBUG_LOG_W(L"Failed to install hook for " << wFunction << L": " << RtlGetLastErrorString());
    }
    else
    {
        std::wstring wFunction(function, function + strlen(function));
        DEBUG_LOG_W(wFunction << L" hook installed.");
        ULONG ACLEntries[1] = { 0 };
        LhSetExclusiveACL(ACLEntries, 1, &hHook);
    }
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);
    if (processId == GetCurrentProcessId()) {
        if (!IsIgnoredHwnd(hwnd)) {
            *(HWND*)lParam = hwnd;
            return FALSE; // Found main window
        }
    }
    return TRUE;
}

void ForceApplyWindowSettings() {
    HWND hMainWindow = NULL;
    EnumWindows(EnumWindowsProc, (LPARAM)&hMainWindow);
    if (hMainWindow) {
        DEBUG_LOG_W(L"Found main window already created, forcefully applying sizes...");
        
        if ((gInjectionInfo.injectionFlags & InjectionFlags::HOOK_FORCE_WINDOW) == InjectionFlags::HOOK_FORCE_WINDOW) {
            DWORD dwStyle = GetWindowLongA(hMainWindow, GWL_STYLE);
            dwStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
            dwStyle |= WS_POPUP;
            SetWindowLongA(hMainWindow, GWL_STYLE, dwStyle);
        }

        int finalX = (gInjectionInfo.windowPosX != CW_USEDEFAULT) ? gInjectionInfo.windowPosX : 0;
        int finalY = (gInjectionInfo.windowPosY != CW_USEDEFAULT) ? gInjectionInfo.windowPosY : 0;
        int finalWidth = (gInjectionInfo.windowSizeX != 0) ? gInjectionInfo.windowSizeX : 800;
        int finalHeight = (gInjectionInfo.windowSizeY != 0) ? gInjectionInfo.windowSizeY : 600;
        
        UINT flags = SWP_NOZORDER | SWP_FRAMECHANGED;
        if (gInjectionInfo.windowPosX == CW_USEDEFAULT && gInjectionInfo.windowPosY == CW_USEDEFAULT) flags |= SWP_NOMOVE;
        if (gInjectionInfo.windowSizeX == 0 && gInjectionInfo.windowSizeY == 0) flags |= SWP_NOSIZE;

        SetWindowPos(hMainWindow, NULL, finalX, finalY, finalWidth, finalHeight, flags);
    } else {
        DEBUG_LOG_W(L"No existing main window found to forcefully resize.");
    }
}

extern "C" void __declspec(dllexport) __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo);

void __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo)
{
    if (inRemoteInfo->UserDataSize != sizeof(InjectionInfo)) return;
    memcpy_s(&gInjectionInfo, sizeof(InjectionInfo), inRemoteInfo->UserData, inRemoteInfo->UserDataSize);

    if ((gInjectionInfo.injectionFlags & InjectionFlags::HOOK_LOGGING) == InjectionFlags::HOOK_LOGGING) {
        g_LoggingEnabled = true;
        Logger::InitializeConsole();
    }

    DEBUG_LOG_W(L"WinSplitPlusIJ Entered");

    bool doAnsi = (gInjectionInfo.injectionFlags & InjectionFlags::HOOK_ANSI) == InjectionFlags::HOOK_ANSI;
    bool doUnicode = (gInjectionInfo.injectionFlags & InjectionFlags::HOOK_UNICODE) == InjectionFlags::HOOK_UNICODE;

    bool doStd = (gInjectionInfo.injectionFlags & InjectionFlags::HOOK_STANDARD) == InjectionFlags::HOOK_STANDARD;
    bool doEx = (gInjectionInfo.injectionFlags & InjectionFlags::HOOK_EXTENDED) == InjectionFlags::HOOK_EXTENDED;

    // Install FindWindow Hooks
    if ((gInjectionInfo.injectionFlags & InjectionFlags::HOOK_FIND_WINDOW) == InjectionFlags::HOOK_FIND_WINDOW)
    {
        if (doAnsi) {
            if (doStd) hookFunction("user32", "FindWindowA", FindWindowAHook);
            if (doEx)  hookFunction("user32", "FindWindowExA", FindWindowExAHook);
        }
        if (doUnicode) {
            if (doStd) hookFunction("user32", "FindWindowW", FindWindowWHook);
            if (doEx)  hookFunction("user32", "FindWindowExW", FindWindowExWHook);
        }
    }

    // Install RegisterClass Hooks
    if ((gInjectionInfo.injectionFlags & InjectionFlags::HOOK_WND_PROC) == InjectionFlags::HOOK_WND_PROC)
    {
        if (doAnsi) {
            if (doStd) hookFunction("user32", "RegisterClassA", RegisterClassAHook);
            if (doEx)  hookFunction("user32", "RegisterClassExA", RegisterClassExAHook);
        }
        if (doUnicode) {
            if (doStd) hookFunction("user32", "RegisterClassW", RegisterClassWHook);
            if (doEx)  hookFunction("user32", "RegisterClassExW", RegisterClassExWHook);
        }
    }

	// Install CreateWindow Hooks
    if ((gInjectionInfo.injectionFlags & InjectionFlags::HOOK_CREATE_WINDOW) == InjectionFlags::HOOK_CREATE_WINDOW)
    {
        if (doAnsi) {
            if (doStd) hookFunction("user32", "CreateWindowA", CreateWindowAHook);
            if (doEx)  hookFunction("user32", "CreateWindowExA", CreateWindowExAHook);
        }
        if (doUnicode) {
            if (doStd) hookFunction("user32", "CreateWindowW", CreateWindowWHook);
            if (doEx)  hookFunction("user32", "CreateWindowExW", CreateWindowExWHook);
        }
    }

	// Install SetWindowPos Hook
    if ((gInjectionInfo.injectionFlags & InjectionFlags::HOOK_SET_WINDOW_POS) == InjectionFlags::HOOK_SET_WINDOW_POS)
    {
        hookFunction("user32", "SetWindowPos", SetWindowPosHook);
    }

    // Install MoveWindow Hook
    if ((gInjectionInfo.injectionFlags & InjectionFlags::HOOK_MOVE_WINDOW) == InjectionFlags::HOOK_MOVE_WINDOW)
    {
        hookFunction("user32", "MoveWindow", MoveWindowHook);
    }

	// Install CreateMutex Hooks
    if ((gInjectionInfo.injectionFlags & InjectionFlags::HOOK_CREATE_MUTEX) == InjectionFlags::HOOK_CREATE_MUTEX)
    {
        if (doAnsi)    hookFunction("Kernel32", "CreateMutexA", CreateMutexAHook);
        if (doUnicode) hookFunction("Kernel32", "CreateMutexW", CreateMutexWHook);
    }

    // Install Display Hooks
    if ((gInjectionInfo.injectionFlags & InjectionFlags::HOOK_FORCE_WINDOW) == InjectionFlags::HOOK_FORCE_WINDOW)
    {
        // These don't take strings, so they ignore doAnsi/doUnicode, but respect Std/Ex
        if (doStd) hookFunction("user32", "AdjustWindowRect", AdjustWindowRectHook);
        if (doEx)  hookFunction("user32", "AdjustWindowRectEx", AdjustWindowRectExHook);

        hookFunction("user32", "GetSystemMetrics", GetSystemMetricsHook);

        if (doAnsi) {
#ifdef _WIN64
            hookFunction("user32", "SetWindowLongPtrA", SetWindowLongPtrAHook);
#else
            hookFunction("user32", "SetWindowLongA", SetWindowLongPtrAHook);
#endif
            if (doStd) hookFunction("user32", "ChangeDisplaySettingsA", ChangeDisplaySettingsAHook);
            if (doEx)  hookFunction("user32", "ChangeDisplaySettingsExA", ChangeDisplaySettingsExAHook);
            hookFunction("user32", "GetMonitorInfoA", GetMonitorInfoAHook);
            hookFunction("user32", "SystemParametersInfoA", SystemParametersInfoAHook);
            hookFunction("user32", "EnumDisplaySettingsA", EnumDisplaySettingsAHook);
        }
        if (doUnicode) {
#ifdef _WIN64
            hookFunction("user32", "SetWindowLongPtrW", SetWindowLongPtrWHook);
#else
            hookFunction("user32", "SetWindowLongW", SetWindowLongPtrWHook);
#endif
            if (doStd) hookFunction("user32", "ChangeDisplaySettingsW", ChangeDisplaySettingsWHook);
            if (doEx)  hookFunction("user32", "ChangeDisplaySettingsExW", ChangeDisplaySettingsExWHook);
            hookFunction("user32", "GetMonitorInfoW", GetMonitorInfoWHook);
            hookFunction("user32", "SystemParametersInfoW", SystemParametersInfoWHook);
            hookFunction("user32", "EnumDisplaySettingsW", EnumDisplaySettingsWHook);
        }
    }

    RhWakeUpProcess();
    
    // Forcefully apply settings to windows that were created BEFORE we injected (late injection)
    ForceApplyWindowSettings();
    
    DEBUG_LOG_W(L"WinSplitPlusIJ Initialization Complete");
}