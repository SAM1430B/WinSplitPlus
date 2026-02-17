/*
This Project is originally written by @SleepKiller from https://www.swbfgamers.com/index.php?topic=11251.msg112827#msg112827 .
And modified by @SAM1430B from splitscreen.me .
*/

#define _WIN32_WINNT 0x0601

#include <Windows.h>
#include <string>
#include <iostream>
#include <vector>
#include <easyhook.h>

#include <algorithm>
#include <cwctype>

#include "InjectionInfo.h"

// Command-line usage information
void print_usage() {
    std::wcout << L"Usage: WinSplitPlus.exe [options] C:\\path\\to\\game.exe [game arguments]\n"
        << L"Options:\n"
        << L"  -Player <Number>        Identifier.\n"
        << L"  -WinClass               Enable Window class hook.\n"
        << L"  -WinName                Enable Window Name hook.\n"
        << L"  -FindWindow             Enable FindWindow hook.\n"
        << L"  -Mutex <Name>           Hook Mutex.\n"
        << L"  -Width <W> -Height <H>  Set Window Size.\n"
        << L"  -Posx <X> -Posy <Y>     Set Window Position.\n\n"
        << L"FILTERS:\n"
        << L"  -A                      Enable ANSI hooks (e.g. RegisterClassA).\n"
        << L"  -W                      Enable Unicode hooks (e.g. RegisterClassW).\n"
        << L"  -Std                    Enable Standard hooks (e.g. RegisterClass).\n"
        << L"  -Ex                     Enable Extended hooks (e.g. RegisterClassEx).\n"
        << L"  -IgnoreWinClass <Name>  Ignore specific Class Name (partial match).\n"
        << L"  -IgnoreWinName <Name>   Ignore specific Window Name (partial match).\n\n"
        << L"  (If no filters are provided, ALL are enabled by default.)\n\n";
}

bool InjectStandardDLL(HANDLE hProcess, const std::wstring& dllPath)
{
    HMODULE hKernel32 = GetModuleHandleW(L"Kernel32");
    if (!hKernel32) return false;

    LPVOID pLoadLibrary = (LPVOID)GetProcAddress(hKernel32, "LoadLibraryW");
    if (!pLoadLibrary) return false;

    size_t bytesNeeded = (dllPath.length() + 1) * sizeof(wchar_t);
    LPVOID pRemoteString = VirtualAllocEx(hProcess, NULL, bytesNeeded, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!pRemoteString) return false;

    if (!WriteProcessMemory(hProcess, pRemoteString, dllPath.c_str(), bytesNeeded, NULL))
    {
        VirtualFreeEx(hProcess, pRemoteString, 0, MEM_RELEASE);
        return false;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibrary, pRemoteString, 0, NULL);
    if (!hThread)
    {
        VirtualFreeEx(hProcess, pRemoteString, 0, MEM_RELEASE);
        return false;
    }

    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(hThread);
    VirtualFreeEx(hProcess, pRemoteString, 0, MEM_RELEASE);

    std::wcout << L"Standard Injection: " << dllPath.substr(dllPath.find_last_of(L"\\/") + 1) << L" - OK" << std::endl;
    return true;
}

// Helper function to convert a string to lowercase
std::wstring to_lower(const std::wstring& str) {
    std::wstring lower_str = str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::towlower);
    return lower_str;
}

int wmain(int argc, wchar_t* argv[])
{
    InjectionInfo injectionInfo = {};
    std::wstring gamePath;
    std::wstring gameArgs;
    std::wstring baseMutexName;
    int playerNumber = 1;
    bool changeWindowName = false;

    bool hasCharsetFilter = false; // -A or -W?
    bool hasVariantFilter = false; // -Std or -Ex?

    if (argc < 2) {
        print_usage();
        system("pause");
        return 1;
    }

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i)
    {
        std::wstring original_arg = argv[i];

        std::wstring lower_arg = to_lower(original_arg);

        if (lower_arg == L"-player" && i + 1 < argc) {
            try {
                playerNumber = std::stoi(argv[++i]);
            }
            catch (const std::exception&) {
                std::wcerr << L"Error: Invalid player number provided." << std::endl;
                Sleep(5000);
                return 1;
            }
        }
        else if (lower_arg == L"-winclass") {
            injectionInfo.injectionFlags = injectionInfo.injectionFlags | InjectionFlags::HOOK_WND_PROC | InjectionFlags::HOOK_CREATE_WINDOW;
        }
        else if (lower_arg == L"-winname") {
            injectionInfo.injectionFlags = injectionInfo.injectionFlags | InjectionFlags::HOOK_CREATE_WINDOW;
            changeWindowName = true;
        }
        else if (lower_arg == L"-findwindow") {
            injectionInfo.injectionFlags = injectionInfo.injectionFlags | InjectionFlags::HOOK_FIND_WINDOW;
        }
        else if (lower_arg == L"-mutex" && i + 1 < argc) {
            injectionInfo.injectionFlags = injectionInfo.injectionFlags | InjectionFlags::HOOK_CREATE_MUTEX;
            baseMutexName = argv[++i];
        }
        else if (lower_arg == L"-width" && i + 1 < argc) {
            injectionInfo.windowSizeX = std::stoul(argv[++i]);
        }
        else if (lower_arg == L"-height" && i + 1 < argc) {
            injectionInfo.windowSizeY = std::stoul(argv[++i]);
        }
        else if (lower_arg == L"-posx" && i + 1 < argc) {
            injectionInfo.windowPosX = std::stoul(argv[++i]);
        }
        else if (lower_arg == L"-posy" && i + 1 < argc) {
            injectionInfo.windowPosY = std::stoul(argv[++i]);
        }
        else if (lower_arg == L"-setwindowpos") {
            injectionInfo.injectionFlags = injectionInfo.injectionFlags | InjectionFlags::HOOK_SET_WINDOW_POS;
        }
        else if (lower_arg == L"-a") {
            injectionInfo.injectionFlags = injectionInfo.injectionFlags | InjectionFlags::HOOK_ANSI;
            hasCharsetFilter = true;
        }
        else if (lower_arg == L"-w") {
            injectionInfo.injectionFlags = injectionInfo.injectionFlags | InjectionFlags::HOOK_UNICODE;
            hasCharsetFilter = true;
        }
        else if (lower_arg == L"-std") {
            injectionInfo.injectionFlags = injectionInfo.injectionFlags | InjectionFlags::HOOK_STANDARD;
            hasVariantFilter = true;
        }
        else if (lower_arg == L"-ex") {
            injectionInfo.injectionFlags = injectionInfo.injectionFlags | InjectionFlags::HOOK_EXTENDED;
            hasVariantFilter = true;
        }
        else if (lower_arg == L"-ignorewinclass" && i + 1 < argc) {
            std::wstring val = argv[++i];
            wcscpy_s(injectionInfo.customIgnoreClassName, IGNORE_MAX_LENGTH, val.c_str());
        }
        else if (lower_arg == L"-ignorewinname" && i + 1 < argc) {
            std::wstring val = argv[++i];
            wcscpy_s(injectionInfo.customIgnoreWindowName, IGNORE_MAX_LENGTH, val.c_str());
        }
        else {
            if (gamePath.empty()) gamePath = original_arg;
            else {
                if (gameArgs.length() > 0) gameArgs += L" ";
                gameArgs += L"\"" + original_arg + L"\"";
            }
        }
    }

    if (gamePath.empty()) {
        std::wcerr << L"Error: Game executable path not provided." << std::endl;
        print_usage();
        system("pause");
        return 1;
    }

    // Construct final names based on Player number
    if (!hasCharsetFilter) {
        injectionInfo.injectionFlags = injectionInfo.injectionFlags | InjectionFlags::HOOK_ANSI | InjectionFlags::HOOK_UNICODE;
        std::wcout << L"Defaulting to ANSI + Unicode." << std::endl;
    }

    if (!hasVariantFilter) {
        injectionInfo.injectionFlags = injectionInfo.injectionFlags | InjectionFlags::HOOK_STANDARD | InjectionFlags::HOOK_EXTENDED;
        std::wcout << L"Defaulting to Standard + Extended variants." << std::endl;
    }

    if ((injectionInfo.injectionFlags & InjectionFlags::HOOK_WND_PROC) == InjectionFlags::HOOK_WND_PROC) {
        std::wstring finalClassName = L"WinSplitPlus" + std::to_wstring(playerNumber);
        wcscpy_s(injectionInfo.windowClassName, CLASS_NAME_MAX_LENGTH, finalClassName.c_str());
    }

    if (changeWindowName) {
        std::wstring finalWindowName = L"WinSplitPlus " + std::to_wstring(playerNumber);
        wcscpy_s(injectionInfo.windowName, WINDOW_NAME_MAX_LENGTH, finalWindowName.c_str());
    }

    if ((injectionInfo.injectionFlags & InjectionFlags::HOOK_CREATE_MUTEX) == InjectionFlags::HOOK_CREATE_MUTEX) {
        if (baseMutexName.empty()) {
            std::wcerr << L"Error: -mutex flag was used, but no base mutex name was provided." << std::endl;
            Sleep(5000);
            return 1;
        }
        wcscpy_s(injectionInfo.mutexOriginalName, MUTEX_NAME_MAX_LENGTH, baseMutexName.c_str());
        std::wstring finalMutexName = baseMutexName + std::to_wstring(playerNumber);
        wcscpy_s(injectionInfo.mutexNewName, MUTEX_NAME_MAX_LENGTH, finalMutexName.c_str());
    }

    // Prepare DLLs
    WCHAR exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);
    std::wstring exeDir = exePath;
    exeDir = exeDir.substr(0, exeDir.find_last_of(L"\\/"));

    std::wstring coreDllName;

#if defined(_WIN64)
    // 64-bit Build
    std::wcout << L"Launcher Build: x64" << std::endl;
    coreDllName = L"WinSplitPlusIJ64.dll";
#else
    // 32-bit Build
    std::wcout << L"Launcher Build: x32 (x86)" << std::endl;
    coreDllName = L"WinSplitPlusIJ32.dll";
#endif

    std::wstring coreDllPath = exeDir + L"\\" + coreDllName;
    if (GetFileAttributesW(coreDllPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        std::wcerr << L"CRITICAL ERROR: Core DLL not found at: " << coreDllPath << std::endl;
        Sleep(10000);
        return 1;
    }

    // Plugin DLLs
    std::vector<std::wstring> pluginDlls;
    std::wstring searchPath = exeDir + L"\\plugins\\*.dll";
    WIN32_FIND_DATA wfd;
    HANDLE hFind = FindFirstFile(searchPath.c_str(), &wfd);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            std::wstring filename = wfd.cFileName;
            std::wstring lowerName = to_lower(filename);
            if (lowerName.length() >= 4 && lowerName.substr(lowerName.length() - 4) == L".dll")
                pluginDlls.push_back(exeDir + L"\\plugins\\" + filename);
        } while (FindNextFile(hFind, &wfd));
        FindClose(hFind);
    }

    // Launch Process
    PROCESS_INFORMATION pi = {};
    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    std::wstring fullCommandLine = L"\"" + gamePath + L"\" " + gameArgs;

    std::wcout << L"Launching game suspended: " << gamePath << std::endl;
    if (!CreateProcess(NULL, const_cast<wchar_t*>(fullCommandLine.c_str()), NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
        std::wcerr << L"Failed to create process." << std::endl;
        return 1;
    }

    std::wcout << L"Resuming process..." << std::endl;
    ResumeThread(pi.hThread);

    std::wcout << L"Attempting to catch process..." << std::endl;

    NTSTATUS nt = -1;
    int attempts = 0;
    const int MAX_ATTEMPTS = 50;

    while (attempts < MAX_ATTEMPTS) {
        nt = RhInjectLibrary(
            pi.dwProcessId,
            0,
            EASYHOOK_INJECT_DEFAULT,
#if defined(_WIN64)
            NULL, const_cast<wchar_t*>(coreDllPath.c_str()),
#else
            const_cast<wchar_t*>(coreDllPath.c_str()), NULL,
#endif
            & injectionInfo, sizeof(InjectionInfo)
        );

        if (nt == 0) {
            std::wcout << L"Injection Successful on attempt " << (attempts + 1) << std::endl;
            break;
        }
        else {
            attempts++;
            Sleep(10);
        }
    }

    if (nt != 0) {
        std::wcerr << L"CRITICAL FAILURE: Could not inject after " << MAX_ATTEMPTS << " attempts." << std::endl;
        std::wcerr << L"Last Error: " << RtlGetLastErrorString() << std::endl;
        // TerminateProcess(pi.hProcess, 1);
        return 1;
    }

    std::wcout << L"Waiting for hooks to settle..." << std::endl;
    Sleep(2000);

    if (!pluginDlls.empty()) {
        std::wcout << L"Injecting plugins..." << std::endl;
        for (const auto& dllPath : pluginDlls) {
            InjectStandardDLL(pi.hProcess, dllPath);
            Sleep(200);
        }
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    std::wcout << L"Success." << std::endl;
    return 0;
}