#pragma once

#include <stdint.h>

/*
    Contains a struct passed to the DLL when it is injected.
*/

enum class InjectionFlags : std::uint64_t
{
    //Use RegisterClassA to hook the main WindowProc of the application.
    HOOK_WND_PROC = (1 << 0),

    //Hook CreateWindowExA, changing the window size, position and name.
    HOOK_CREATE_WINDOW = (1 << 1),

    //Hook SetWindowPos, suppressing attempts to change the window's position.
    HOOK_SET_WINDOW_POS = (1 << 2),

    //Hook CreateMutexA, changing a mutex's name.
    HOOK_CREATE_MUTEX = (1 << 8),

};

//Operator overloads for InjectionFlags
inline InjectionFlags operator|(InjectionFlags a, InjectionFlags b) { return static_cast<InjectionFlags>(static_cast<uint64_t>(a) | static_cast<uint64_t>(b)); };
inline InjectionFlags operator&(InjectionFlags a, InjectionFlags b) { return static_cast<InjectionFlags>(static_cast<uint64_t>(a) & static_cast<uint64_t>(b)); };
inline InjectionFlags operator^(InjectionFlags a, InjectionFlags b) { return static_cast<InjectionFlags>(static_cast<uint64_t>(a) ^ static_cast<uint64_t>(b)); };

const std::size_t CLASS_NAME_MAX_LENGTH = 256;
const std::size_t WINDOW_NAME_MAX_LENGTH = 512;
const std::size_t MUTEX_NAME_MAX_LENGTH = 260;

struct InjectionInfo
{
    InjectionFlags injectionFlags = (InjectionFlags)0;

    // If HOOK_WND_PROC is set, the class name of the Window is set to this.
    wchar_t windowClassName[CLASS_NAME_MAX_LENGTH]{};

    // Window dimensions and position
    std::uint32_t windowSizeX = 0;
    std::uint32_t windowSizeY = 0;
    std::uint32_t windowPosX = 0;
    std::uint32_t windowPosY = 0;

    // If HOOK_CREATE_WINDOW is set, the title of the Window is set to this.
    wchar_t windowName[WINDOW_NAME_MAX_LENGTH]{};

    // If HOOK_CREATE_MUTEX is set, the mutex name is changed.
    wchar_t mutexOriginalName[MUTEX_NAME_MAX_LENGTH]{};
    wchar_t mutexNewName[MUTEX_NAME_MAX_LENGTH]{};
};