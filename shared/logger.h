#pragma once

class logger
{
private:
    static void log(const char* text, const char* level, const char* color)
    {
        std::cout << color << "[" << level << "]" << "\033[0m" << ": " << text << std::endl;
    }

public:
    static void info(const char* format, ...)
    {
        char buffer[512];
        va_list args;
        va_start(args, format);
        std::vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);

        log(buffer, "info", "\033[34m");
    }

    static void warn(const char* format, ...)
    {
        char buffer[512];
        va_list args;
        va_start(args, format);
        std::vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);

        log(buffer, "warn", "\033[33m");
    }

    static void error(const char* format, ...)
    {
        char buffer[512];
        va_list args;
        va_start(args, format);
        std::vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);

        log(buffer, "error", "\033[31m");
    }
};