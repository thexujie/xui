#include "stdafx.h"
#include "string.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace core
{

	//std::vector<std::string_view> split(const std::string_view & src, const char & delimiter)
	//{
	//	std::vector<std::string_view> tokens;
	//	std::string token;

	//	int last = 0;
	//	int curr = 0;
	//	while (curr < src.length())
	//	{
	//		if (src[curr] == delimiter)
	//		{
	//			tokens.push_back(src.substr(last, curr));
	//			last = curr + 1;
	//		}
	//		++curr;
	//	}
	//	if (last <= curr)
	//		tokens.push_back(src.substr(last, curr));
	//	return tokens;
	//}


    //std::string_view & ltrim(std::string_view & str, const std::string & chars)
    //{
    //    str = str.substr(str.find_first_not_of(chars));
    //    return str;
    //}

    //std::string_view & rtrim(std::string_view & str, const std::string & chars)
    //{
    //    str = str.substr(0, str.find_last_not_of(chars) + 1);
    //    return str;
    //}

    //std::string_view & trim(std::string_view & str, const std::string & chars)
    //{
    //    return ltrim(rtrim(str, chars), chars);
    //}

    std::u8string astr_u8str(const char* astr, size_t nchar)
    {
        if (nchar == npos)
            nchar = std::strlen(astr);

    	int32_t nwchar_need = MultiByteToWideChar(CP_ACP, 0, astr, (int)nchar, NULL, 0);
		std::wstring wstr(nwchar_need, 0);
        int32_t nwchar_result = MultiByteToWideChar(CP_ACP, 0, astr, (int)nchar, wstr.data(), nwchar_need);
        assert(nwchar_result == nwchar_need);

        int32_t nchar_need = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), nwchar_result, NULL, 0, NULL, FALSE);
		std::u8string u8str(nchar_need, 0);
        int32_t nchar_result = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), nwchar_result, reinterpret_cast<char *>(u8str.data()), nchar_need, NULL, FALSE);
        assert(nchar_result == nchar_need);
        return u8str;
    }

    std::string u8str_astr(const char8_t * u8str, size_t nu8char)
    {
        if (nu8char == npos)
            nu8char = std::strlen(reinterpret_cast<const char *>(u8str));

    	int32_t nwchar_need = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char *>(u8str), (int)nu8char, NULL, 0);
		std::wstring wstr(nwchar_need, 0);
        int32_t nwchar = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char *>(u8str), (int)nu8char, wstr.data(), nwchar_need);
        assert(nwchar == nwchar_need);

        int32_t nchar_need = WideCharToMultiByte(CP_ACP, 0, wstr.data(), nwchar, NULL, 0, NULL, FALSE);
		std::string astr(nchar_need, 0);
        int32_t nchar_result = WideCharToMultiByte(CP_ACP, 0, wstr.data(), nwchar, astr.data(), nchar_need, NULL, FALSE);
        assert(nchar_result == nchar_need);
        return astr;
    }

    std::u8string wstr_u8str(const wchar_t * wstr, size_t nwchar)
    {
        if (nwchar == npos)
            nwchar = std::wcslen(wstr);
        int32_t nchar_need = WideCharToMultiByte(CP_UTF8, 0, wstr, (int)nwchar, NULL, 0, NULL, FALSE);
		std::u8string u8str(nchar_need, 0);
        int32_t nchar_result = WideCharToMultiByte(CP_UTF8, 0, wstr, (int)nwchar, reinterpret_cast<char *>(u8str.data()), nchar_need, NULL, FALSE);
        assert(nchar_result == nchar_need);
        return u8str;
    }

    std::wstring u8str_wstr(const char8_t * u8str, size_t nu8char)
    {
        if (nu8char == npos)
            nu8char = std::strlen(reinterpret_cast<const char *>(u8str));
        int32_t nchar_need = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char *>(u8str), (int)nu8char, NULL, 0);
		std::wstring wstr(nchar_need, 0);
        int32_t nchar_result = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char *>(u8str), (int)nu8char, wstr.data(), nchar_need);
        assert(nchar_result == nchar_need);
        return wstr;
    }

    std::string wstr_astr(const wchar_t * wstr, size_t nwchar)
    {
        if (nwchar == npos)
            nwchar = std::wcslen(wstr);

        int32_t nchar_need = WideCharToMultiByte(CP_ACP, 0, wstr, (int)nwchar, NULL, 0, NULL, FALSE);
		std::string astr(nchar_need, 0);
        int32_t nchar_result = WideCharToMultiByte(CP_ACP, 0, wstr, (int)nwchar, astr.data(), nchar_need, NULL, FALSE);
        assert(nchar_result == nchar_need);
        return astr;
    }

    std::wstring astr_wstr(const char * astr, size_t nchar)
    {
        if (nchar == npos)
            nchar = std::strlen(astr);
        int32_t nchar_need = MultiByteToWideChar(CP_ACP, 0, astr, (int)nchar, NULL, 0);
		std::wstring wstr(nchar_need, 0);
        int32_t nchar_result = MultiByteToWideChar(CP_ACP, 0, astr, (int)nchar, wstr.data(), nchar_need);
        assert(nchar_result == nchar_need);
        return wstr;
    }

    std::u8string from_bytes(std::shared_ptr<byte_t> bytes, int32_t nbytes)
    {
        std::u8string str;
        const char8_t chars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
        for (int32_t cnt = 0; cnt < nbytes; ++cnt)
        {
            byte_t b = bytes.get()[cnt];
            str.append(1, chars[(b >> 4) & 0xf]);
            str.append(1, chars[b & 0xf]);
        }
        return str;
    }
}
