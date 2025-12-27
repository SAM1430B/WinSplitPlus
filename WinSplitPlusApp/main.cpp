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

// Helper function to convert a string to lowercase
std::wstring to_lower(const std::wstring& str) {
    std::wstring lower_str = str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::towlower);
    return lower_str;
}

// Command-line usage information
void print_usage() {
    std::wcout << L"Usage: WinSplitPlus.exe [options] C:\\path\\to\\game.exe [game arguments]\n"
        << L"Options:\n"
        << L"  -Player <Number>        Identifier.\n"
        << L"  -WinClass               Enable Window class hook.\n"
        << L"  -WinName                Enable Window Name hook.\n"
        << L"  -Mutex <Name>           Hook Mutex.\n"
        << L"  -Width <W> -Height <H>  Set Window Size.\n"
        << L"  -Posx <X> -Posy <Y>     Set Window Position.\n\n"
        << L"FILTERS:\n"
        << L"  -A                      Enable ANSI hooks (e.g. RegisterClassA).\n"
        << L"  -W                      Enable Unicode hooks (e.g. RegisterClassW).\n"
        << L"  -Std                    Enable Standard hooks (e.g. RegisterClass).\n"
        << L"  -Ex                     Enable Extended hooks (e.g. RegisterClassEx).\n\n"
        << L"  (If no filters are provided, ALL are enabled by default.)\n\n";
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

    // Inject the DLL
    WCHAR exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);
    std::wstring exeDir = exePath;
    size_t lastSlash = exeDir.find_last_of(L"\\/");
    if (lastSlash != std::wstring::npos) {
        exeDir = exeDir.substr(0, lastSlash);
    }

    std::vector<std::wstring> dllsToInject;

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

    // Add the Core DLL to the top of the list
    std::wstring coreDllPath = exeDir + L"\\" + coreDllName;

    if (GetFileAttributesW(coreDllPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
        dllsToInject.push_back(coreDllPath);
        std::wcout << L"Added Core DLL: " << coreDllName << std::endl;
    }
    else {
        std::wcerr << L"CRITICAL ERROR: Core DLL not found at: " << coreDllPath << std::endl;
        Sleep(10000);
        return 1;
    }

    std::wstring pluginsDir = exeDir + L"\\plugins\\";
    std::wstring searchPath = pluginsDir + L"*.dll";
    WIN32_FIND_DATA wfd;
    HANDLE hFind = FindFirstFile(searchPath.c_str(), &wfd);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            std::wstring dllName = wfd.cFileName;
            dllsToInject.push_back(pluginsDir + dllName);
        } while (FindNextFile(hFind, &wfd));
        FindClose(hFind);
    }
    else {
        std::wcout << L"No plugins found in " << pluginsDir << L" (Only Core DLL will be injected)." << std::endl;
    }


    // Suspended state is needed for Window hooks
    PROCESS_INFORMATION pi = {};
    STARTUPINFOW si = {};
    si.cb = sizeof(si);

    std::wstring fullCommandLine = L"\"" + gamePath + L"\" " + gameArgs;

    std::wcout << L"Launching game suspended: " << gamePath << std::endl;
    std::wcout << L"Injecting " << dllsToInject.size() << L" DLLs..." << std::endl;

    if (!CreateProcess(
        NULL,
        const_cast<wchar_t*>(fullCommandLine.c_str()), // Command line
        NULL,
        NULL,
        FALSE,
        CREATE_SUSPENDED,
        NULL,
        NULL,
        &si, // StartupInfo
        &pi // ProcessInfo
    )) {
        std::wcerr << L"Failed to create process: " << GetLastError() << std::endl;
        std::wcerr << L"Command: " << fullCommandLine << std::endl;
        Sleep(7000);
        return 1;
    }

    DWORD processId = pi.dwProcessId;
    bool allInjectionsSucceeded = true;

    for (const auto& dllPath : dllsToInject) {
        std::wcout << L"Injecting " << dllPath.substr(dllPath.find_last_of(L"\\/") + 1) << L"..." << std::endl;

        NTSTATUS nt = RhInjectLibrary(
            processId,
            0,
            EASYHOOK_INJECT_DEFAULT,
            const_cast<wchar_t*>(dllPath.c_str()),
            const_cast<wchar_t*>(dllPath.c_str()),
            &injectionInfo,
            sizeof(InjectionInfo)
        );

        if (nt != 0) {
            std::wcerr << L"Failed to inject DLL " << dllPath << L": " << RtlGetLastErrorString() << std::endl;
            allInjectionsSucceeded = false;
        }
    }

    if (!allInjectionsSucceeded) {
        std::wcerr << L"Warning: One or more DLLs failed to inject." << std::endl;
        Sleep(5000);
    }
    else {
        std::wcout << L"All injections successful." << std::endl;
    }

    std::wcout << L"Resuming game process..." << std::endl;
    ResumeThread(pi.hThread);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    std::wcout << L"Successfully launched and injected into process " << processId << L". The launcher will now exit." << std::endl;

    return 0;
}