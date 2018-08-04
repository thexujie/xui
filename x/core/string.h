#pragma once
#include "utils.h"

namespace core { namespace string
{
    std::vector<std::string> split(const std::string & src, const char8_t & delimiter);
    std::vector<std::string_view> split(const std::string_view & src, const char8_t & delimiter);
    std::string & ltrim(std::string & str, const std::string & chars = "\t\n\v\f\r ");
    std::string & rtrim(std::string & str, const std::string & chars = "\t\n\v\f\r ");
    std::string & trim(std::string & str, const std::string & chars = "\t\n\v\f\r ");
    std::string_view & ltrim(std::string_view & str, const std::string & chars = "\t\n\v\f\r ");
    std::string_view & rtrim(std::string_view & str, const std::string & chars = "\t\n\v\f\r ");
    std::string_view & trim(std::string_view & str, const std::string & chars = "\t\n\v\f\r ");

    std::string ansi_u8(const char8_t * text, int32_t length);
    std::string u8_ansi(const char8_t * text, int32_t length);
    std::string u16_u8(const char16_t * text, int32_t length);
    std::string u16_ansi(const char16_t * text, int32_t length);
    std::wstring u8_u16(const char8_t * text, int32_t length);
    std::wstring ansi_u16(const char8_t * text, int32_t length);

    std::string ansi_u8(std::string str);
    std::string u8_ansi(std::string str);
    std::string usc2_u8(std::wstring str);
    std::string usc2_ansi(std::wstring str);
    std::wstring u8_u16(std::string str);
    std::string u16_u8(std::wstring str);

    struct less_ic
    {
        bool operator()(const std::string & s1, const std::string & s2) const
        {
            return core::textcmp(s1.c_str(), s1.length(), s2.c_str(), s2.length(), false) < 0;
        }
    };

    inline bool equal_ic(const std::string & s1, const std::string & s2)
    {
        if (s1.length() != s2.length())
            return false;

        return core::textequalex(s1.c_str(), s1.length(), s2.c_str(), s2.length(), false);
    }

    inline bool equal_ic(const std::string_view & s1, const char * s2, int32_t s2_length)
    {
        if (s2_length < 0)
            s2_length = textlen(s2);

        if (s1.length() != s2_length)
            return false;

        return core::textequalex(s1.data(), s1.length(), s2, s2_length, false);
    }

    inline bool equal_ic(const std::string & s1, const char * s2, int32_t s2_length)
    {
        if (s2_length < 0)
            s2_length = textlen(s2);

        if (s1.length() != s2_length)
            return false;

        return core::textequalex(s1.c_str(), s1.length(), s2, s2_length, false);
    }

    inline void format_helper(std::ostringstream & stream) {}

    template<typename Head, typename ...Tail>
    void format_helper(std::ostringstream & stream, const Head & head, Tail && ...tail)
    {
        stream << head;
        return format_helper(stream, std::forward<Tail>(tail)...);
    }

    template<typename ...Args>
    std::string format(Args && ...args)
    {
        std::ostringstream stream;
        format_helper(stream, std::forward<Args>(args)...);
        return stream.str();
    }

    std::string from_bytes(std::shared_ptr<byte_t> bytes, int32_t nbytes);
}}
