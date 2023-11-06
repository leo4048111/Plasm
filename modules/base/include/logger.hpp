#pragma once

#include <cstdarg>

#include "base.hpp"
#include "singleton.hpp"

// Usage LogD(1, "+", "2", '=', 3);
#define PrintLog2(level, ...) SOLIDITY2CPN_NM::Logger::GetInstance().PrintLog(level, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOGT(...) PrintLog2(SOLIDITY2CPN_NM::LogLevel::LTrace, ##__VA_ARGS__)
#define LOGV(...) PrintLog2(SOLIDITY2CPN_NM::LogLevel::LVerbose, ##__VA_ARGS__)
#define LOGD(...) PrintLog2(SOLIDITY2CPN_NM::LogLevel::LDebug, ##__VA_ARGS__)
#define LOGI(...) PrintLog2(SOLIDITY2CPN_NM::LogLevel::LInfo, ##__VA_ARGS__)
#define LOGW(...) PrintLog2(SOLIDITY2CPN_NM::LogLevel::LWarn, ##__VA_ARGS__)
#define LOGE(...) PrintLog2(SOLIDITY2CPN_NM::LogLevel::LError, ##__VA_ARGS__)

_START_SOLIDITY2CPN_NM_ 

enum class LogLevel {
    LTrace = 0,
    LVerbose,
    LDebug,
    LInfo,
    LWarn,
    LError
};

class Logger {
    SOLIDITY2CPN_SINGLETON(Logger)

public:
    void PrintLog(LogLevel level, const char *file,
                         const char *function, int line, const char *fmt, ...);
    void PrintLogV(LogLevel level, const char *file,
                          const char *function, int line, const char *fmt,
                          va_list ap);
};

_END_SOLIDITY2CPN_NM_