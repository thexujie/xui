#include "stdafx.h"
#include <cstdarg>
#include "core.h"

namespace core
{
    void breakpoint()
    {
        //_asm int 3
        __debugbreak();
    }

    int32_t textformat_calc(const char8_t * format, ...)
    {
        va_list args = nullptr;
        va_start(args, format);
        int32_t iLength = textformatargs_calc(format, args);
        va_end(args);
        return iLength;
    }

    int32_t textformat_calc(const char16_t * format, ...)
    {
        va_list args = nullptr;
        va_start(args, format);
        int32_t iLength = textformatargs_calc(format, args);
        va_end(args);
        return iLength;
    }

    int32_t textformat(char8_t * buffer, int32_t size, const char8_t * format, ...)
    {
        va_list args = nullptr;
        va_start(args, format);
        int32_t iLength = textformat_args(buffer, size, format, args);
        va_end(args);
        return iLength;
    }

    int32_t textformat(char16_t * buffer, int32_t size, const char16_t * format, ...)
    {
        va_list args = nullptr;
        va_start(args, format);
        int32_t iLength = textformat_args(buffer, size, format, args);
        va_end(args);
        return iLength;
    }

    int32_t textformatargs_calc(const char8_t * format, void * args)
    {
        return _vscprintf((const char *)format, (va_list)args) + 1;
    }

    int32_t textformatargs_calc(const char16_t * format, void * args)
    {
        return _vscwprintf((const wchar_t *)format, (va_list)args) + 1;
    }

    int32_t textformat_args(char8_t * buffer, int32_t size, const char8_t * format, void * args)
    {
        return vsprintf_s((char *)buffer, size, (const char *)format, (va_list)args);
    }

    int32_t textformat_args(char16_t * buffer, int32_t size, const char16_t * format, void * args)
    {
        return vswprintf_s((wchar_t *)buffer, size, (const wchar_t *)format, (va_list)args);
    }

    bool isprintchar(char32_t ch)
    {
        if (ch == 0x7F)
            return false;
        else if (ch >= 0xFF)
            return true;
        else if (ch == L'\t')
            return true;
        else if (ch >= L' ' && ch <= L'~')
            return true;
        else
            return false;
    }
}
