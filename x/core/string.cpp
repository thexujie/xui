#include "stdafx.h"
#include "string.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace core { namespace string
{
    std::vector<std::string> split(const std::string & src, const char & delimiter)
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream sstream(src);
        while (std::getline(sstream, token, delimiter)) { tokens.push_back(token); }
        return tokens;
    }

    std::vector<std::string_view> split(const std::string_view & src, const char & delimiter)
    {
        std::vector<std::string_view> tokens;
        std::string token;

        int last = 0;
        int curr = 0;
        while (curr < src.length())
        {
            if (src[curr] == delimiter)
            {
                tokens.push_back(src.substr(last, curr));
                last = curr + 1;
            }
            ++curr;
        }
        if (last <= curr)
            tokens.push_back(src.substr(last, curr));
        return tokens;
    }

    std::string & ltrim(std::string & str, const std::string & chars)
    {
        str.erase(0, str.find_first_not_of(chars));
        return str;
    }

    std::string & rtrim(std::string & str, const std::string & chars)
    {
        str.erase(str.find_last_not_of(chars) + 1);
        return str;
    }

    std::string & trim(std::string & str, const std::string & chars) { return ltrim(rtrim(str, chars), chars); }


    std::string_view & ltrim(std::string_view & str, const std::string & chars)
    {
        str = str.substr(str.find_first_not_of(chars));
        return str;
    }

    std::string_view & rtrim(std::string_view & str, const std::string & chars)
    {
        str = str.substr(0, str.find_last_not_of(chars) + 1);
        return str;
    }

    std::string_view & trim(std::string_view & str, const std::string & chars)
    {
        return ltrim(rtrim(str, chars), chars);
    }

    std::string ansi_u8(const char * text, int32_t length)
    {
        if (length < 0)
            length = (int32_t)std::strlen(text);
#ifdef USE_UTF8
        int32_t nwchars = MultiByteToWideChar(CP_ACP, 0, text, length, NULL, 0);
        wchar_t * u16 = new wchar_t[nwchars];
        int32_t nwchars2 = MultiByteToWideChar(CP_ACP, 0, text, length, u16, nwchars);
        assert(nwchars2 == nwchars);

        int32_t nchars = WideCharToMultiByte(CP_UTF8, 0, u16, nwchars2, NULL, 0, NULL, FALSE);
        char * u8 = new char[nchars];
        int32_t nchars2 = WideCharToMultiByte(CP_UTF8, 0, u16, nwchars2, u8, nchars, NULL, FALSE);
        delete[] u16;
        assert(nchars2 == nchars);
        std::string str(u8, nchars);
        delete[] u8;
        return str;
#else
        return std::string(text, length);
#endif
    }

    std::string u8_ansi(const char * text, int32_t length)
    {
        if (length < 0)
            length = (int32_t)std::strlen(text);
#ifdef USE_UTF8
        int32_t nwchars = MultiByteToWideChar(CP_UTF8, 0, text, length, NULL, 0);
        wchar_t * u16 = new wchar_t[nwchars];
        int32_t nwchars2 = MultiByteToWideChar(CP_UTF8, 0, text, length, u16, nwchars);
        assert(nwchars2 == nwchars);

        int32_t nchars = WideCharToMultiByte(CP_ACP, 0, u16, nwchars2, NULL, 0, NULL, FALSE);
        char * out = new char[nchars];
        int32_t nchars2 = WideCharToMultiByte(CP_ACP, 0, u16, nwchars2, out, nchars, NULL, FALSE);
        delete[] u16;
        assert(nchars2 == nchars);
        std::string str(out, nchars);
        delete[] out;
        return str;
#else
        return std::string(text, length);
#endif
    }

    std::string u16_u8(const wchar_t * text, int32_t length)
    {
        if (length < 0)
            length = (int32_t)std::wcslen(text);
#ifdef USE_UTF8
        int32_t nchars = WideCharToMultiByte(CP_UTF8, 0, text, length, NULL, 0, NULL, FALSE);
#else
        int32_t nchars = WideCharToMultiByte(CP_ACP, 0, text, length, NULL, 0, NULL, FALSE);
#endif
        char * u8 = new char[nchars];
#ifdef USE_UTF8
        int32_t nchars2 = WideCharToMultiByte(CP_UTF8, 0, text, length, u8, nchars, NULL, FALSE);
#else
        int32_t nchars2 = WideCharToMultiByte(CP_ACP, 0, text, length, u8, nchars, NULL, FALSE);
#endif
        assert(nchars2 == nchars);
        std::string str(u8, nchars);
        delete[] u8;
        return str;
    }

    std::wstring u8_u16(const char * text, int32_t length)
    {
        if (length < 0)
            length = (int32_t)std::strlen(text);
#ifdef USE_UTF8
        int32_t nchars = MultiByteToWideChar(CP_UTF8, 0, text, length, NULL, 0);
#else
        int32_t nchars = MultiByteToWideChar(CP_ACP, 0, text, length, NULL, 0);
#endif
        wchar_t * u16 = new wchar_t[nchars];
        int32_t nchars2 = MultiByteToWideChar(CP_UTF8, 0, text, length, u16, nchars);
        assert(nchars2 == nchars);
        std::wstring ucs2(u16, nchars);
        delete[] u16;
        return ucs2;
    }

    std::string u16_ansi(const wchar_t * text, int32_t length)
    {
        if (length < 0)
            length = (int32_t)std::wcslen(text);

        int32_t nchars = WideCharToMultiByte(CP_ACP, 0, text, length, NULL, 0, NULL, FALSE);
        char * u8 = new char[nchars];
        int32_t nchars2 = WideCharToMultiByte(CP_ACP, 0, text, length, u8, nchars, NULL, FALSE);
        assert(nchars2 == nchars);
        std::string str(u8, nchars);
        delete[] u8;
        return str;
    }

    std::wstring ansi_u16(const char * text, int32_t length)
    {
        if (length < 0)
            length = int32_t(std::strlen(text));
        int32_t nchars = MultiByteToWideChar(CP_ACP, 0, text, length, NULL, 0);
        wchar_t * u16 = new wchar_t[nchars];
        int32_t nchars2 = MultiByteToWideChar(CP_ACP, 0, text, length, u16, nchars);
        assert(nchars2 == nchars);
        std::wstring ucs2(u16, nchars);
        delete[] u16;
        return ucs2;
    }

    std::string ansi_u8(std::string str)
    {
        return ansi_u8(str.c_str(), (int32_t)str.length());
    }

    std::string u8_ansi(std::string str)
    {
        return u8_ansi(str.c_str(), (int32_t)str.length());
    }

    std::string u16_u8(std::wstring str)
    {
        return u16_u8(str.c_str(), (int32_t)str.length());
    }

    std::wstring u8_u16(std::string str)
    {
        return u8_u16(str.c_str(), (int32_t)str.length());
    }

    std::string u16_ansi(std::wstring str)
    {
        return u16_ansi(str.c_str(), (int32_t)str.length());
    }

    std::wstring ansi_u16(std::string str)
    {
        return ansi_u16(str.c_str(), (int32_t)str.length());
    }

    std::string from_bytes(std::shared_ptr<byte_t> bytes, int32_t nbytes)
    {
        std::string str;
        const char chars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
        for (int32_t cnt = 0; cnt < nbytes; ++cnt)
        {
            byte_t b = bytes.get()[cnt];
            str.append(1, chars[(b >> 4) & 0xf]);
            str.append(1, chars[b & 0xf]);
        }
        return str;
    }
}}
