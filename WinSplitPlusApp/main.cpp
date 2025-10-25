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

#include "InectionInfo.h"

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
        << L"  -Player <Number>       Identifier for this game instance (appended to names).\n"
        << L"  -winclass              Enable Window class hook.\n"
        << L"  -winname               Enable Window Name hook.\n"
        << L"  -mutex <baseMutexName> Set the base name for the game's mutex.\n"
        << L"  -width <width>         Set the window width.\n"
        << L"  -height <height>       Set the window height.\n"
        << L"  -posx <x>              Set the window X position.\n"
        << L"  -posy <y>              Set the window Y position.\n\n"
        << L"  Command Line Example:\n\n"
        << L"  WinSplitPlus.exe -Player 1 -winclass -winname -mutex FarCryMutex -width 1280 -height 720 -posx 100 -posy 100\n"
        << L"  \"C:\\Games\\FarCry\\Bin32\\FarCry.exe\" -window\n\n";
}

int wmain(int argc, wchar_t* argv[])
{
    InjectionInfo injectionInfo = {};
    std::wstring gamePath;
    std::wstring gameArgs;
    std::wstring baseMutexName;
    int playerNumber = 1;

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
            injectionInfo.injectionFlags = injectionInfo.injectionFlags | InjectionFlags::HOOK_WND_PROC;
        }
        else if (lower_arg == L"-winname") {
            injectionInfo.injectionFlags = injectionInfo.injectionFlags | InjectionFlags::HOOK_CREATE_WINDOW;
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
        else {
            if (gamePath.empty()) {
                gamePath = original_arg;
            }
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
    if ((injectionInfo.injectionFlags & InjectionFlags::HOOK_WND_PROC) == InjectionFlags::HOOK_WND_PROC) {
        std::wstring finalClassName = L"WinSplitPlus" + std::to_wstring(playerNumber);
        wcscpy_s(injectionInfo.windowClassName, CLASS_NAME_MAX_LENGTH, finalClassName.c_str());
    }

    if ((injectionInfo.injectionFlags & InjectionFlags::HOOK_CREATE_WINDOW) == InjectionFlags::HOOK_CREATE_WINDOW) {
        std::wstring finalWindowName = L"WinSplitPlus " + std::to_wstring(playerNumber);
        wcscpy_s(injectionInfo.windowName, WINDOW_NAME_MAX_LENGTH, finalWindowName.c_str());
    }

    if ((injectionInfo.injectionFlags & InjectionFlags::HOOK_CREATE_MUTEX) == InjectionFlags::HOOK_CREATE_MUTEX) {
        if (baseMutexName.empty()) {
            std::wcerr << L"Error: -mutex flag was used, but no base mutex name was provided." << std::endl;
            Sleep(3000);
            return 1;
        }
        wcscpy_s(injectionInfo.mutexOriginalName, MUTEX_NAME_MAX_LENGTH, baseMutexName.c_str());
        std::wstring finalMutexName = baseMutexName + std::to_wstring(playerNumber);
        wcscpy_s(injectionInfo.mutexNewName, MUTEX_NAME_MAX_LENGTH, finalMutexName.c_str());
    }

    if (injectionInfo.windowSizeX > 0 || injectionInfo.windowSizeY > 0 || injectionInfo.windowPosX > 0 || injectionInfo.windowPosY > 0) {
        injectionInfo.injectionFlags = injectionInfo.injectionFlags | InjectionFlags::HOOK_SET_WINDOW_POS;
    }

    // Inject the DLL
    WCHAR dllToInject[] = L"WinSplitPlusIJ.dll";
    DWORD processId = 0;

    std::wcout << L"Launching game: " << gamePath << std::endl;
    std::wcout << L"Injecting with settings for Player " << playerNumber << L"..." << std::endl;

    NTSTATUS nt = RhCreateAndInject(
        const_cast<wchar_t*>(gamePath.c_str()),
        gameArgs.empty() ? NULL : const_cast<wchar_t*>(gameArgs.c_str()),
        0, EASYHOOK_INJECT_DEFAULT, dllToInject, NULL,
        &injectionInfo, sizeof(InjectionInfo), &processId
    );

    if (nt != 0) {
        std::wcerr << L"Failed to inject DLL: " << RtlGetLastErrorString() << std::endl;
        Sleep(5000);
        return 1;
    }

    std::wcout << L"Successfully launched and injected into process " << processId << L". The launcher will now exit." << std::endl;

    return 0;
}