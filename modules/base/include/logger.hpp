#pragma once

#include <cstdarg>

#include "base.hpp"
#include "singleton.hpp"

// Usage LogD(1, "+", "2", '=', 3);
#define PrintLog2(level, ...) PSM_NM::Logger::GetInstance().PrintLog(level, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOGT(...) PrintLog2(PSM_NM::LogLevel::LTrace, ##__VA_ARGS__)
#define LOGV(...) PrintLog2(PSM_NM::LogLevel::LVerbose, ##__VA_ARGS__)
#define LOGD(...) PrintLog2(PSM_NM::LogLevel::LDebug, ##__VA_ARGS__)
#define LOGI(...) PrintLog2(PSM_NM::LogLevel::LInfo, ##__VA_ARGS__)
#define LOGW(...) PrintLog2(PSM_NM::LogLevel::LWarn, ##__VA_ARGS__)
#define LOGE(...) PrintLog2(PSM_NM::LogLevel::LError, ##__VA_ARGS__)

_START_PSM_NM_

enum class LogLevel {
    LTrace = 0,
    LVerbose,
    LDebug,
    LInfo,
    LWarn,
    LError
};

class Logger {
    PSM_SINGLETON(Logger)

public:
    void PrintLog(LogLevel level, const char *file,
                         const char *function, int line, const char *fmt, ...);
    void PrintLogV(LogLevel level, const char *file,
                          const char *function, int line, const char *fmt,
                          va_list ap);
    void SetLevel(LogLevel level);

private:
    LogLevel level_{LogLevel::LInfo};
};

_END_PSM_NM_