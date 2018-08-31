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
    std::string u16_u8(std::wstring str);
    std::string u16_ansi(std::wstring str);
    std::wstring u8_u16(std::string str);
    std::string u16_u8(std::wstring str);

    struct less_ic
    {
        bool operator()(const std::string & s1, const std::string & s2) const
        {
            if (s1.length() != s2.length())
                return s1.length() < s2.length();
            else
            {
                for(size_t i = 0; i < s1.length(); ++i)
                {
                    auto c1 = std::tolower(s1[i]);
                    auto c2 = std::tolower(s2[i]);
                    if (c1 == c2)
                        continue;
                    return c1 < c2;
                }
                return false;
            }
        }
    };

    inline bool char_equal(const char & c1, const char & c2) { return c1 == c2; }
    inline bool char_equal_ic(const char & c1, const char & c2) { return std::tolower(c1) == std::tolower(c2); }
    inline bool equal(const char * s1, size_t s1_length, const char * s2, size_t s2_length = npos)
    {
        if (s1_length == npos)
            s1_length = core::textlen(s1);
        if (s2_length == npos)
            s2_length = core::textlen(s2);
        return std::equal(s1, s1 + s1_length, s2, s2 + s2_length, char_equal);
    }

    inline bool equal(const std::string & s1, const std::string & s2)
    {
        return std::equal(s1.begin(), s1.end(), s2.begin(), s2.end(), char_equal);
    }

    inline bool equal(const std::string_view & s1, const char * s2, size_t s2_length = npos)
    {
        if (s2_length == npos)
            s2_length = core::textlen(s2);
        return std::equal(s1.begin(), s1.end(), s2, s2 + s2_length, char_equal);
    }

    inline bool equal(const std::string & s1, const char * s2, size_t s2_length = npos)
    {
        if (s2_length == npos)
            s2_length = core::textlen(s2);
        return std::equal(s1.begin(), s1.end(), s2, s2 + s2_length, char_equal);
    }


    inline bool equal_ic(const char * s1, size_t s1_length, const char * s2, size_t s2_length = npos)
    {
        if (s1_length == npos)
            s1_length = core::textlen(s1);
        if (s2_length == npos)
            s2_length = core::textlen(s2);
        return std::equal(s1, s1 + s1_length, s2, s2 + s2_length, char_equal_ic);
    }

    inline bool equal_ic(const std::string & s1, const std::string & s2)
    {
        return std::equal(s1.begin(), s1.end(), s2.begin(), s2.end(), char_equal_ic);
    }

    inline bool equal_ic(const std::string_view & s1, const char * s2, size_t s2_length = npos)
    {
        if (s2_length == npos)
            s2_length = core::textlen(s2);
        return std::equal(s1.begin(), s1.end(), s2, s2 + s2_length, char_equal_ic);
    }

    inline bool equal_ic(const std::string & s1, const char * s2, size_t s2_length = npos)
    {
        if (s2_length == npos)
            s2_length = core::textlen(s2);
        return std::equal(s1.begin(), s1.end(), s2, s2 + s2_length, char_equal_ic);
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
