#include "logger.hpp"

#include <iostream>
#include <cstdarg>
#include <cstdio>
#include <string>

_START_SOLIDITY2CPN_NM_

void Logger::PrintLog(LogLevel level, const char *file,
                      const char *function, int line, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    PrintLogV(level, file, function, line, fmt, ap);
    va_end(ap);
}

void Logger::PrintLogV(LogLevel level, const char *file, const char *function, int line, const char *fmt, va_list ap)
{
    LogLevel msgLevel = level;
    static const char *COLORS[] = {
        "\033[94m", // Blue for TRACE
        "\033[96m", // Cyan for VERBOSE
        "\033[95m", // Magenta for DEBUG
        "\033[92m", // Green for INFO
        "\033[93m", // Yellow for WARN
        "\033[91m"  // Red for ERROR
    };
    static const char* RESET = "\033[0m";
    char *formattedStr = nullptr;
    if (vasprintf(&formattedStr, fmt, ap) >= 0 && formattedStr)
    {
        // Create a prefix with the log level, file, function, and line information
        const char *levelStr = "";
        switch (msgLevel)
        {
        case LogLevel::LTrace:
            levelStr = "TRACE";
            break;
        case LogLevel::LVerbose:
            levelStr = "VERBOSE";
            break;
        case LogLevel::LDebug:
            levelStr = "DEBUG";
            break;
        case LogLevel::LInfo:
            levelStr = "INFO";
            break;
        case LogLevel::LWarn:
            levelStr = "WARN";
            break;
        case LogLevel::LError:
            levelStr = "ERROR";
            break;
        }

        std::string logMsg = COLORS[(int)msgLevel] + std::string(levelStr) + ": " + formattedStr + RESET;
        logMsg = "[" + std::string(file) + ":" + std::string(function) + ":" + std::to_string(line) + "] " + logMsg;

        // Print the log message
        std::cout << logMsg << std::endl;

        // Free the memory allocated by vasprintf
        free(formattedStr);
    }
    else
    {
        // Allocation failed, handle the error as appropriate
        std::cerr << "Failed to allocate memory for log message" << std::endl;
    }
}

_END_SOLIDITY2CPN_NM_