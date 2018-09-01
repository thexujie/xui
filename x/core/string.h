#pragma once
#include "utils.h"

namespace core
{
    std::vector<std::string> split(const std::string & src, const char8_t & delimiter);
    std::vector<std::string_view> split(const std::string_view & src, const char8_t & delimiter);
    std::string & ltrim(std::string & str, const std::string & chars = "\t\n\v\f\r ");
    std::string & rtrim(std::string & str, const std::string & chars = "\t\n\v\f\r ");
    std::string & trim(std::string & str, const std::string & chars = "\t\n\v\f\r ");
    std::string_view & ltrim(std::string_view & str, const std::string & chars = "\t\n\v\f\r ");
    std::string_view & rtrim(std::string_view & str, const std::string & chars = "\t\n\v\f\r ");
    std::string_view & trim(std::string_view & str, const std::string & chars = "\t\n\v\f\r ");

    std::string astr_u8str(const char8_t * text, size_t length);
    std::string u8str_astr(const char8_t * text, size_t length);
    inline std::string astr_u8str(std::string str) { return astr_u8str(str.c_str(), str.length()); }
    inline std::string u8str_astr(std::string str) { return u8str_astr(str.c_str(), str.length()); }

    std::string wstr_u8str(const wchar_t * text, size_t length);
    std::wstring u8str_wstr(const char8_t * text, size_t length);
    inline std::string wstr_u8str(std::wstring str) { return wstr_u8str(str.c_str(), str.length()); }
    inline std::wstring u8str_wstr(std::string str) { return u8str_wstr(str.c_str(), str.length()); }

    std::string wstr_astr(const wchar_t * text, size_t length);
    std::wstring astr_wstr(const char8_t * text, size_t length);
    inline std::string wstr_astr(std::wstring str) { return wstr_astr(str.c_str(), str.length()); }
    inline std::wstring astr_wstr(std::string str) { return astr_wstr(str.c_str(), str.length()); }

    inline std::string u16str_u8str(const char16_t * text, size_t length) { return wstr_u8str(reinterpret_cast<const wchar_t *>(text), length); }
    inline std::u16string u8str_u16str(const char8_t * text, size_t length) { auto wstr = u8str_wstr(text, length); return std::u16string(wstr.begin(), wstr.end()); }
    inline std::string u16str_u8str(std::u16string str) { return u16str_u8str(str.c_str(), str.length()); }
    inline std::u16string u8str_u16str(std::string str) { return u8str_u16str(str.c_str(), str.length()); }

    inline std::string u16str_astr(const char16_t * text, size_t length) { return wstr_astr(reinterpret_cast<const wchar_t *>(text), length); }
    inline std::u16string astr_u16str(const char8_t * text, size_t length) { auto wstr = astr_wstr(text, length); return std::u16string(wstr.begin(), wstr.end()); }
    inline std::string u16str_astr(std::u16string str) { return u16str_astr(str.c_str(), str.length()); }
    inline std::u16string astr_u16str(std::string str) { return astr_u16str(str.c_str(), str.length()); }

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
}
