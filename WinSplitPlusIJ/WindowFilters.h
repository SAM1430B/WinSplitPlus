#pragma once
#include <Windows.h>
#include <WinUser.h>
#include <string>

#include "include\InjectionInfo.h"

extern InjectionInfo gInjectionInfo;

// FILTER LISTS
static const char* IGNORED_MODULES[] = {
    "protoinput", "gtomnk", "dinput8", "xinput", "d3d9", "d3d11", "dxgi",
    "gameoverlayrenderer", "discordhook", "rtss", "specialk", "windowmovehook"
};

static const char* IGNORED_CLASSES[] = {
    "GtoMnK", "ProtoInput", "PROTORAWINPUT", "PROTOINPUTUI", "PROTOHOST",
    "dummy", "SK_BackgroundWindow", "SK_HID_Listener", "Kiero", "RTSS",
    "Special K", "EOSOVH", "CurseOverlay", "TestDX11", "SKIV_", "InvisibleWindow",
    "TempDirect3D", "TempWindow", "DDrawCompat", "Overlay", "MSXML", "DIEmWin",
    "DirectSound", "Direct3D", "DirectDraw", "RawInput", "SDL_", "Message",
    "IME", "MSCTF", "static"
};

static const wchar_t* IGNORED_CLASSES_W[] = {
    L"GtoMnK", L"ProtoInput", L"PROTORAWINPUT", L"PROTOINPUTUI", L"PROTOHOST",
    L"dummy", L"SK_BackgroundWindow", L"SK_HID_Listener", L"Kiero", L"RTSS",
    L"Special K", L"EOSOVH", L"CurseOverlay", L"TestDX11", L"SKIV_", L"InvisibleWindow",
    L"TempDirect3D", L"TempWindow", L"DDrawCompat", L"Overlay", L"MSXML", L"DIEmWin",
    L"DirectSound", L"Direct3D", L"DirectDraw", L"RawInput", L"SDL_", L"Message",
    L"IME", L"MSCTF", L"static"
};

// HELPER FUNCTIONS
inline bool ContainsIgnoreCaseA(const char* haystack, const char* needle) {
    if (!haystack || !needle) return false;
    size_t needleLen = strlen(needle);
    while (*haystack) {
        if (_strnicmp(haystack, needle, needleLen) == 0) return true;
        haystack++;
    }
    return false;
}

inline bool ContainsIgnoreCaseW(const wchar_t* haystack, const wchar_t* needle) {
    if (!haystack || !needle) return false;
    size_t needleLen = wcslen(needle);
    while (*haystack) {
        if (_wcsnicmp(haystack, needle, needleLen) == 0) return true;
        haystack++;
    }
    return false;
}

inline bool IgnoreClassA(LPCSTR lpClassName) {
    if (lpClassName == NULL || IS_INTRESOURCE(lpClassName)) return false;

    if (gInjectionInfo.customIgnoreClassName[0] != L'\0') {
        char ansiCustom[IGNORE_MAX_LENGTH];
        wcstombs_s(nullptr, ansiCustom, IGNORE_MAX_LENGTH, gInjectionInfo.customIgnoreClassName, _TRUNCATE);
        if (ContainsIgnoreCaseA(lpClassName, ansiCustom)) return true;
    }

    for (const char* ignoredClass : IGNORED_CLASSES) {
        if (ContainsIgnoreCaseA(lpClassName, ignoredClass)) return true;
    }
    return false;
}

inline bool IgnoreClassW(LPCWSTR lpClassName) {
    if (lpClassName == NULL || IS_INTRESOURCE(lpClassName)) return false;

    if (gInjectionInfo.customIgnoreClassName[0] != L'\0') {
        if (ContainsIgnoreCaseW(lpClassName, gInjectionInfo.customIgnoreClassName)) return true;
    }

    for (const wchar_t* ignoredClass : IGNORED_CLASSES_W) {
        if (ContainsIgnoreCaseW(lpClassName, ignoredClass)) return true;
    }
    return false;
}

inline bool IsThreadIgnored() {
    bool isIgnored = false;
    EnumThreadWindows(GetCurrentThreadId(), [](HWND hwnd, LPARAM lParam) -> BOOL {
        char className[256] = { 0 };
        if (GetClassNameA(hwnd, className, sizeof(className))) {
            for (const char* ignoredClass : IGNORED_CLASSES) {
                if (ContainsIgnoreCaseA(className, ignoredClass)) {
                    *(bool*)lParam = true;
                    return FALSE;
                }
            }
        }
        return TRUE;
        }, (LPARAM)&isIgnored);
    return isIgnored;
}

inline bool IsCallerIgnoredModule() {
    PVOID frames[16];
    WORD framesCount = RtlCaptureStackBackTrace(1, 15, frames, NULL);

    for (WORD i = 0; i < framesCount; i++) {
        HMODULE hModule = NULL;
        if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCSTR)frames[i], &hModule)) {
            char modulePath[MAX_PATH] = { 0 };
            if (GetModuleFileNameA(hModule, modulePath, sizeof(modulePath))) {
                char* fileName = strrchr(modulePath, '\\');
                fileName = fileName ? fileName + 1 : modulePath;

                for (const char* ignoredMod : IGNORED_MODULES) {
                    if (ContainsIgnoreCaseA(fileName, ignoredMod)) return true;
                }
            }
        }
    }
    return IsThreadIgnored();
}

inline bool IsIgnoredHwnd(HWND hWnd) {
    if (!hWnd) return false;

    DWORD dwExStyle = GetWindowLongA(hWnd, GWL_EXSTYLE);
    if (dwExStyle & WS_EX_TOOLWINDOW) return true;

    char className[256] = { 0 };
    if (GetClassNameA(hWnd, className, sizeof(className))) {
        if (IgnoreClassA(className)) return true;
    }

    char windowName[256] = { 0 };
    GetWindowTextA(hWnd, windowName, sizeof(windowName));

    if (windowName[0] != '\0' && gInjectionInfo.customIgnoreWindowName[0] != L'\0') {
        char ansiCustom[IGNORE_MAX_LENGTH];
        wcstombs_s(nullptr, ansiCustom, IGNORE_MAX_LENGTH, gInjectionInfo.customIgnoreWindowName, _TRUNCATE);
        if (ContainsIgnoreCaseA(windowName, ansiCustom)) return true;
    }
    return false;
}