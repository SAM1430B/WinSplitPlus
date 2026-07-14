#pragma once

#include <stdint.h>

enum class InjectionFlags : std::uint64_t
{
    HOOK_WND_PROC = (1 << 0),       // Enable Class Registration hooks
    HOOK_CREATE_WINDOW = (1 << 1),  // Enable CreateWindow hooks
    HOOK_SET_WINDOW_POS = (1 << 2), // Enable SetWindowPos hooks
    HOOK_MOVE_WINDOW = (1 << 3),    // Enable MoveWindow hooks

    HOOK_ANSI = (1 << 4),           // Enable *A hooks
    HOOK_UNICODE = (1 << 5),        // Enable *W hooks
    HOOK_EXTENDED = (1 << 6),       // Enable *Ex hooks
    HOOK_STANDARD = (1 << 7),       // Enable Standard hooks

    HOOK_FIND_WINDOW = (1 << 8),    // FindWindow hooks

	// TODO: Add support for these hooks
    HOOK_CREATE_MUTEX = (1 << 9),   // Enable Mutex hooks
    //HOOK_CREATE_EVENT = (1 << 10),   // Enable Event hooks
    //HOOK_CREATE_SEMAPHORE = (1 << 11), // Enable Semaphore hooks

    HOOK_CREATE_MUTEX = (1 << 8),   // Enable Mutex hooks
};

// Operator overloads
inline InjectionFlags operator|(InjectionFlags a, InjectionFlags b) { return static_cast<InjectionFlags>(static_cast<uint64_t>(a) | static_cast<uint64_t>(b)); };
inline InjectionFlags operator&(InjectionFlags a, InjectionFlags b) { return static_cast<InjectionFlags>(static_cast<uint64_t>(a) & static_cast<uint64_t>(b)); };
inline InjectionFlags operator^(InjectionFlags a, InjectionFlags b) { return static_cast<InjectionFlags>(static_cast<uint64_t>(a) ^ static_cast<uint64_t>(b)); };
inline InjectionFlags operator~(InjectionFlags a) { return static_cast<InjectionFlags>(~static_cast<uint64_t>(a)); };

const std::size_t CLASS_NAME_MAX_LENGTH = 256;
const std::size_t WINDOW_NAME_MAX_LENGTH = 512;
const std::size_t MUTEX_NAME_MAX_LENGTH = 260;
const int IGNORE_MAX_LENGTH = 64;

struct InjectionInfo
{
    InjectionFlags injectionFlags = (InjectionFlags)0;

    wchar_t windowClassName[CLASS_NAME_MAX_LENGTH]{};
    wchar_t windowName[WINDOW_NAME_MAX_LENGTH]{};

    std::uint32_t windowSizeX = 0;
    std::uint32_t windowSizeY = 0;
    std::uint32_t windowPosX = 0;
    std::uint32_t windowPosY = 0;
    
    wchar_t mutexOriginalName[MUTEX_NAME_MAX_LENGTH]{};
    wchar_t mutexNewName[MUTEX_NAME_MAX_LENGTH]{};

    wchar_t customIgnoreClassName[IGNORE_MAX_LENGTH] = { 0 };
    wchar_t customIgnoreWindowName[IGNORE_MAX_LENGTH] = { 0 };
};