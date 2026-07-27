#pragma once

#include <Windows.h>
#include <sstream>
#include <iostream>
#include <fstream>

extern bool g_LoggingEnabled;

#ifndef LOG_PREFIX
#define LOG_PREFIX "[WinSplitPlus]"
#endif

#ifndef LOG_PREFIX_W
#define LOG_PREFIX_W L"[WinSplitPlus]"
#endif

#define DEBUG_LOG(msg) \
    do { \
        if (g_LoggingEnabled) { \
            std::ostringstream os; \
            os << LOG_PREFIX << " " << msg << "\n"; \
            OutputDebugStringA(os.str().c_str()); \
            std::ofstream logFile("WinSplitPlus.log", std::ios::app); \
            if (logFile.is_open()) { \
                logFile << os.str(); \
                logFile.close(); \
            } \
        } \
    } while (0)

#define DEBUG_LOG_W(msg) \
    do { \
        if (g_LoggingEnabled) { \
            std::wostringstream os; \
            os << LOG_PREFIX_W << L" " << msg << L"\n"; \
            OutputDebugStringW(os.str().c_str()); \
            std::wofstream logFile(L"WinSplitPlus.log", std::ios::app); \
            if (logFile.is_open()) { \
                logFile << os.str(); \
                logFile.close(); \
            } \
        } \
    } while (0)

namespace Logger {
    inline void InitializeConsole() {
        if (g_LoggingEnabled) {
            std::ofstream logFile("WinSplitPlus.log", std::ios::app);
            if (logFile.is_open()) {
                logFile << "------------------------------------------------------------\n";
                logFile << "[WinSplitPlus] Logging Session Started\n";
                logFile << "------------------------------------------------------------\n";
                logFile.close();
            }
        }
    }
}
