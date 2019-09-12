#pragma once
#include "utils.h"

namespace std
{
	using u8ios           = basic_ios<char8_t, char_traits<char8_t>>;
	using u8streambuf     = basic_streambuf<char8_t, char_traits<char8_t>>;
	using u8istream       = basic_istream<char8_t, char_traits<char8_t>>;
	using u8ostream       = basic_ostream<char8_t, char_traits<char8_t>>;
	using u8iostream      = basic_iostream<char8_t, char_traits<char8_t>>;
	using u8stringbuf     = basic_stringbuf<char8_t, char_traits<char8_t>, allocator<char8_t>>;
	using u8istringstream = basic_istringstream<char8_t, char_traits<char8_t>, allocator<char8_t>>;
	using u8ostringstream = basic_ostringstream<char8_t, char_traits<char8_t>, allocator<char8_t>>;
	using u8stringstream  = basic_stringstream<char8_t, char_traits<char8_t>, allocator<char8_t>>;
	using u8filebuf       = basic_filebuf<char8_t, char_traits<char8_t>>;
	using u8ifstream      = basic_ifstream<char8_t, char_traits<char8_t>>;
	using u8ofstream      = basic_ofstream<char8_t, char_traits<char8_t>>;
	using u8fstream       = basic_fstream<char8_t, char_traits<char8_t>>;
}

namespace core
{
	template<typename char_type = char>
	std::basic_string_view<char_type> getline(const std::basic_string_view<char_type> & str, size_t & offset, char_type delimiter)
	{
		size_t begin = offset;
		size_t count = 0;
		while (offset < str.length())
		{
			if (str[offset++] == delimiter)
				break;

			++count;
		}
		return std::basic_string_view<char_type>(str.data() + begin, count);

	}
	template<typename char_type = char>
	std::basic_string_view<char_type> getline(const std::basic_string_view<char_type> & str, size_t & offset, const std::basic_string_view<char_type> & delimiters)
	{
		size_t begin = offset;
		size_t count = 0;
		while (offset < str.length())
		{
			if (delimiters.find_first_of(str[offset++]) != std::basic_string_view<char_type>::npos)
				break;

			++count;
		}
		return std::basic_string_view<char_type>(str.data() + begin, count);
	}
	
	template<typename char_type>
	std::vector<std::basic_string_view<char_type>> split(const std::basic_string_view<char_type> & str, const std::basic_string_view<char_type> & delimiters)
	{
		std::vector<std::basic_string_view<char_type>> tokens;
		size_t offset = 0;
		while (true)
		{
			auto token = core::getline(str, offset, delimiters);
			if (!token.length())
				break;
			tokens.push_back(token);
		}
		return tokens;
	}

	template<typename char_type>
	std::vector<std::basic_string_view<char_type>> split(const std::basic_string_view<char_type> & str, char_type delimiter)
	{
		std::vector<std::basic_string_view<char_type>> tokens;
		size_t offset = 0;
		while (true)
		{
			auto token = core::getline(str, offset, delimiter);
			if (!token.length())
				break;
			tokens.push_back(token);
		}
		return tokens;
	}

	template <class T>
	const T trim_chars = T();

	template <>
	const std::string trim_chars<std::string> = "\t\n\v\f\r ";

#ifdef __cpp_char8_t
	template <>
	const std::u8string trim_chars<std::u8string> = u8"\t\n\v\f\r ";
#endif

	template <>
	const std::wstring trim_chars<std::wstring> = L"\t\n\v\f\r ";

	template<typename string_t = std::string>
	string_t & ltrim(string_t & str, const string_t & chars = trim_chars<string_t>)
	{
		str.erase(0, str.find_first_not_of(chars));
		return str;
	}

	template<typename string_t = std::string>
	string_t & rtrim(string_t & str, const string_t & chars = trim_chars<string_t>)
	{
		str.erase(str.find_last_not_of(chars) + 1);
		return str;
	}

	template<typename string_t = std::string>
	string_t & trim(string_t & str, const string_t & chars = trim_chars<string_t>) { return ltrim(rtrim(str, chars), chars); }


	template<typename T>
	inline bool char_equal(const T & c1, const T & c2) { return c1 == c2; }
	template<typename T>
	inline bool char_equal_ic(const T & c1, const T & c2) { return std::tolower((int)c1) == std::tolower((int)c2); }

	template<typename string_t = std::string>
	inline bool equal(const string_t & s1, const typename string_t::value_type * s2, size_t s2_length = string_t::npos)
	{
		if (s2_length == string_t::npos)
			s2_length = core::textlen(s2);

		return std::equal(s1.begin(), s1.end(), s2, s2 + s2_length, char_equal<typename string_t::value_type>);
	}

	template<typename T>
	inline bool equal(const T * s1, size_t s1_length, const T * s2, size_t s2_length = core::npos)
	{
		if (s1_length == core::npos)
			s1_length = core::textlen(s1);

		if (s2_length == core::npos)
			s2_length = core::textlen(s2);

		if (s1_length != s2_length)
			return false;

		return std::equal(s1, s1 + s2_length, s2, s2 + s2_length, char_equal<T>);
	}

	template<typename string_t = std::string>
	inline bool equal(const string_t & s1, const string_t & s2)
	{
		return std::equal(s1.begin(), s1.end(), s2.begin(), s2.end(), char_equal<typename string_t::value_type>);
	}

	template<typename string_t = std::string>
	inline bool equal_ic(const string_t & s1, const typename string_t::value_type * s2, size_t s2_length = string_t::npos)
	{
		if (s2_length == string_t::npos)
			s2_length = core::textlen(s2);

		return std::equal(s1.begin(), s1.end(), s2, s2 + s2_length, char_equal_ic<typename string_t::value_type>);
	}

	template<typename T>
	inline bool equal_ic(const T * s1, size_t s1_length, const T * s2, size_t s2_length = core::npos)
	{
		if (s1_length == core::npos)
			s1_length = core::textlen(s1);

		if (s2_length == core::npos)
			s2_length = core::textlen(s2);

		if (s1_length != s2_length)
			return false;

		return std::equal(s1, s1 + s2_length, s2, s2 + s2_length, char_equal_ic<T>);
	}

	template<typename string_t = std::string>
	inline bool equal_ic(const string_t & s1, const string_t & s2)
	{
		return std::equal(s1.begin(), s1.end(), s2.begin(), s2.end(), char_equal_ic<typename string_t::value_type>);
	}


    std::u8string astr_u8str(const char * astr, size_t nchar = core::npos);
    std::string u8str_astr(const char8_t * u8str, size_t nu8char = core::npos);
	inline std::u8string astr_u8str(const std::string & astr) { return astr_u8str(astr.c_str(), astr.length()); }
	inline std::u8string astr_u8str(const std::string_view & astr) { return astr_u8str(astr.data(), astr.length()); }
    inline std::string u8str_astr(const std::u8string & astr) { return u8str_astr(astr.c_str(), astr.length()); }
	inline std::string u8str_astr(const std::u8string_view & u8str) { return u8str_astr(u8str.data(), u8str.length()); }

    std::u8string wstr_u8str(const wchar_t * wstr, size_t nwchar = core::npos);
    std::wstring u8str_wstr(const char8_t * u8str, size_t nu8char = core::npos);
    inline std::u8string wstr_u8str(const std::wstring & wstr) { return wstr_u8str(wstr.c_str(), wstr.length()); }
	inline std::wstring u8str_wstr(const std::u8string & u8str) { return u8str_wstr(u8str.c_str(), u8str.length()); }
	inline std::wstring u8str_wstr(const std::u8string_view & u8str) { return u8str_wstr(u8str.data(), u8str.length()); }

    std::string wstr_astr(const wchar_t * wstr, size_t nwchar = core::npos);
    std::wstring astr_wstr(const char * astr, size_t nchar = core::npos);
    inline std::string wstr_astr(const std::wstring & wstr) { return wstr_astr(wstr.c_str(), wstr.length()); }
    inline std::wstring astr_wstr(const std::string & astr) { return astr_wstr(astr.c_str(), astr.length()); }

    inline std::u8string u16str_u8str(const char16_t * u16str, size_t nu16char = core::npos) { return wstr_u8str(reinterpret_cast<const wchar_t *>(u16str), nu16char); }
    inline std::u16string u8str_u16str(const char8_t * u8str, size_t nu8char = core::npos) { auto wstr = u8str_wstr(u8str, nu8char); return std::u16string(wstr.begin(), wstr.end()); }
    inline std::u8string u16str_u8str(const std::u16string & u16str) { return u16str_u8str(u16str.c_str(), u16str.length()); }
    inline std::u16string u8str_u16str(const std::u8string & u8str) { return u8str_u16str(u8str.c_str(), u8str.length()); }

    inline std::string u16str_astr(const char16_t * u16str, size_t nu16char = core::npos) { return wstr_astr(reinterpret_cast<const wchar_t *>(u16str), nu16char); }
    inline std::u16string astr_u16str(const char * astr, size_t nchar = core::npos) { auto wstr = astr_wstr(astr, nchar); return std::u16string(wstr.begin(), wstr.end()); }
    inline std::string u16str_astr(const std::u16string & u16str) { return u16str_astr(u16str.c_str(), u16str.length()); }
    inline std::u16string astr_u16str(const std::string & astr) { return astr_u16str(astr.c_str(), astr.length()); }

	inline std::wstring u16str_wstr(const char16_t * u16str, size_t nu16char = core::npos) { if (nu16char == core::npos) nu16char = core::textlen(u16str);  return std::wstring(reinterpret_cast<const wchar_t *>(u16str), nu16char); }
	inline std::u16string wstr_u16str(const wchar_t * wstr, size_t nwchar = core::npos) { if (nwchar == core::npos) nwchar = core::textlen(wstr); return std::u16string(reinterpret_cast<const char16_t *>(wstr), nwchar); }
	inline std::wstring u16str_wstr(const std::u16string & u16str) { return std::wstring(reinterpret_cast<const wchar_t *>(u16str.c_str()), u16str.length()); }
	inline std::u16string wstr_u16str(const std::wstring & wstr) { return std::u16string(reinterpret_cast<const char16_t *>(wstr.c_str()), wstr.length()); }

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

    std::u8string from_bytes(std::shared_ptr<byte_t> bytes, int32_t nbytes);
}

namespace core
{
	// std::u8string
	inline std::u8ostream & operator<<(std::u8ostream & ost, const char * astr)
	{
		if (!ost.good())
			return ost;
		if (!astr)
			return ost;
		else
			return std::operator <<(ost, astr);
	}

	inline std::u8ostream & operator<<(std::u8ostream && ost, const char * astr)
	{
		if (!ost.good())
			return ost;
		if (!astr)
			return ost;
		else
			return std::operator <<(ost, astr);
	}

#ifdef __cpp_char8_t
	inline std::u8ostream & operator<<(std::u8ostream & ost, const char8_t * u8str)
	{
		if (!ost.good())
			return ost;
		if (!u8str)
			return ost;
		else
			return std::operator <<(ost, u8str);
	}

	inline std::u8ostream & operator<<(std::u8ostream && ost, const char8_t * u8str)
	{
		if (!ost.good())
			return ost;
		if (!u8str)
			return ost;
		else
			return std::operator <<(ost, u8str);
	}
#endif
	inline std::u8ostream & operator<<(std::u8ostream & ost, const wchar_t * wstr)
	{
		if (!ost.good())
			return ost;
		if (!wstr)
			return ost;
		else
			return ost << core::wstr_u8str(wstr);
	}

	inline std::u8ostream & operator<<(std::u8ostream && ost, const wchar_t * wstr)
	{
		if (!ost.good())
			return ost;
		if (!wstr)
			return ost;
		else
			return ost << core::wstr_u8str(wstr);
	}

#ifdef __cpp_char8_t
	inline std::u8ostream & operator<<(std::u8ostream & ost, const std::string & astr)
	{
		if (!ost.good())
			return ost;
		return ost << reinterpret_cast<const std::u8string &>(astr);
	}

	inline std::u8ostream & operator<<(std::u8ostream && ost, const std::string & astr)
	{
		if (!ost.good())
			return ost;
		return ost << reinterpret_cast<const std::u8string &>(astr);
	}

	inline std::u8ostream & operator<<(std::u8ostream & ost, const std::string_view & astr_view)
	{
		if (!ost.good())
			return ost;
		return ost << reinterpret_cast<const std::u8string_view &>(astr_view);
	}

	inline std::u8ostream & operator<<(std::u8ostream && ost, const std::string_view & astr_view)
	{
		if (!ost.good())
			return ost;
		return ost << reinterpret_cast<const std::u8string_view &>(astr_view);
	}
#endif

	inline std::u8ostream & operator<<(std::u8ostream & ost, const std::wstring & wstr)
	{
		if (!ost.good())
			return ost;
		return ost << core::wstr_u8str(wstr);
	}

	inline std::u8ostream & operator<<(std::u8ostream && ost, const std::wstring & wstr)
	{
		if (!ost.good())
			return ost;
		return ost << core::wstr_u8str(wstr);
	}
	inline std::u8ostream & operator<<(std::u8ostream & ost, const std::wstring_view & wstrview)
	{
		if (!ost.good())
			return ost;
		return ost << core::wstr_u8str(wstrview.data(), wstrview.length());
	}

	inline std::u8ostream & operator<<(std::u8ostream && ost, const std::wstring_view & wstrview)
	{
		if (!ost.good())
			return ost;
		return ost << core::wstr_u8str(wstrview.data(), wstrview.length());
	}

	inline std::u8ostream & operator<<(std::u8ostream & ost, const std::u16string & u16str)
	{
		if (!ost.good())
			return ost;
		return ost << core::u16str_u8str(u16str);
	}

	inline std::u8ostream & operator<<(std::u8ostream && ost, const std::u16string & u16str)
	{
		if (!ost.good())
			return ost;
		return ost << core::u16str_u8str(u16str);
	}


	inline std::u8ostream & operator<<(std::u8ostream & ost, const std::u16string_view & u16strview)
	{
		if (!ost.good())
			return ost;
		return ost << core::u16str_u8str(u16strview.data(), u16strview.length());
	}

	inline std::u8ostream & operator<<(std::u8ostream && ost, const std::u16string_view & u16strview)
	{
		if (!ost.good())
			return ost;
		return ost << core::u16str_u8str(u16strview.data(), u16strview.length());
	}


	//---------------------------- std::string

#ifdef __cpp_char8_t
	inline std::ostream & operator<<(std::ostream & ost, const char * astr)
	{
		if (!ost.good())
			return ost;
		if (!astr)
			return ost;
		else
			return std::operator <<(ost, astr);
	}

	inline std::ostream & operator<<(std::ostream && ost, const char * astr)
	{
		if (!ost.good())
			return ost;
		if (!astr)
			return ost;
		else
			return std::operator <<(ost, astr);
	}

	inline std::ostream & operator<<(std::ostream & ost, const char8_t * u8str)
	{
		if (!ost.good())
			return ost;
		if (!u8str)
			return ost;
		else
			return std::operator <<(ost, reinterpret_cast<const char *>(u8str));
	}

	inline std::ostream & operator<<(std::ostream && ost, const char8_t * u8str)
	{
		if (!ost.good())
			return ost;
		if (!u8str)
			return ost;
		else
			return std::operator <<(ost, reinterpret_cast<const char *>(u8str));
	}
	inline std::ostream & operator<<(std::ostream & ost, const wchar_t * wstr)
	{
		if (!ost.good())
			return ost;
		if (!wstr)
			return ost;
		else
			return ost << core::wstr_astr(wstr);
	}

	inline std::ostream & operator<<(std::ostream && ost, const wchar_t * wstr)
	{
		if (!ost.good())
			return ost;
		if (!wstr)
			return ost;
		else
			return ost << core::wstr_astr(wstr);
	}

	inline std::ostream & operator<<(std::ostream & ost, const std::u8string & u8str)
	{
		if (!ost.good())
			return ost;
		return ost << reinterpret_cast<const std::string &>(u8str);
	}

	inline std::ostream & operator<<(std::ostream && ost, const std::u8string & u8str)
	{
		if (!ost.good())
			return ost;
		return ost << reinterpret_cast<const std::string &>(u8str);
	}

	inline std::ostream & operator<<(std::ostream & ost, const std::u8string_view & u8str_view)
	{
		if (!ost.good())
			return ost;
		return ost << reinterpret_cast<const std::string_view &>(u8str_view);
	}

	inline std::ostream & operator<<(std::ostream && ost, const std::u8string_view & u8str_view)
	{
		if (!ost.good())
			return ost;
		return ost << reinterpret_cast<const std::string_view &>(u8str_view);
	}

	inline std::ostream & operator<<(std::ostream & ost, const std::wstring & wstr)
	{
		if (!ost.good())
			return ost;
		return ost << core::wstr_astr(wstr);
	}

	inline std::ostream & operator<<(std::ostream && ost, const std::wstring & wstr)
	{
		if (!ost.good())
			return ost;
		return ost << core::wstr_astr(wstr);
	}
	inline std::ostream & operator<<(std::ostream & ost, const std::wstring_view & wstrview)
	{
		if (!ost.good())
			return ost;
		return ost << core::wstr_astr(wstrview.data(), wstrview.length());
	}

	inline std::ostream & operator<<(std::ostream && ost, const std::wstring_view & wstrview)
	{
		if (!ost.good())
			return ost;
		return ost << core::wstr_astr(wstrview.data(), wstrview.length());
	}

	inline std::ostream & operator<<(std::ostream & ost, const std::u16string & u16str)
	{
		if (!ost.good())
			return ost;
		return ost << core::u16str_wstr(u16str);
	}

	inline std::ostream & operator<<(std::ostream && ost, const std::u16string & u16str)
	{
		if (!ost.good())
			return ost;
		return ost << core::u16str_wstr(u16str);
	}


	inline std::ostream & operator<<(std::ostream & ost, const std::u16string_view & u16strview)
	{
		if (!ost.good())
			return ost;
		return ost << core::u16str_wstr(u16strview.data(), u16strview.length());
	}

	inline std::ostream & operator<<(std::ostream && ost, const std::u16string_view & u16strview)
	{
		if (!ost.good())
			return ost;
		return ost << core::u16str_wstr(u16strview.data(), u16strview.length());
	}
#endif

	//--------------------- wstring

	inline std::wostream & operator<<(std::wostream & ost, const char * astr)
	{
		if (!ost.good())
			return ost;
		if (!astr)
			return ost;
		else
			return ost << core::astr_wstr(astr);
	}

	inline std::wostream & operator<<(std::wostream && ost, const char * astr)
	{
		if (!ost.good())
			return ost;
		if (!astr)
			return ost;
		else
			return ost << core::astr_wstr(astr);
	}
#ifdef __cpp_char8_t
	inline std::wostream & operator<<(std::wostream & ost, const char8_t * u8str)
	{
		if (!ost.good())
			return ost;
		if (!u8str)
			return ost;
		else
			return ost << core::u8str_wstr(u8str);
	}

	inline std::wostream & operator<<(std::wostream && ost, const char8_t * u8str)
	{
		if (!ost.good())
			return ost;
		if (!u8str)
			return ost;
		else
			return ost << core::u8str_wstr(u8str);
	}
#endif
	inline std::wostream & operator<<(std::wostream & ost, const wchar_t * wstr)
	{
		if (!ost.good())
			return ost;
		if (!wstr)
			return ost;
		else
			return std::operator <<(ost, wstr);
	}

	inline std::wostream & operator<<(std::wostream && ost, const wchar_t * wstr)
	{
		if (!ost.good())
			return ost;
		if (!wstr)
			return ost;
		else
			return std::operator <<(ost, wstr);
	}

	inline std::wostream & operator<<(std::wostream & ost, const std::string & astr)
	{
		if (!ost.good())
			return ost;
		return ost << core::astr_wstr(astr);
	}

	inline std::wostream & operator<<(std::wostream && ost, const std::string & astr)
	{
		if (!ost.good())
			return ost;
		return ost << core::astr_wstr(astr);
	}
	inline std::wostream & operator<<(std::wostream & ost, const std::string_view & a8str_view)
	{
		if (!ost.good())
			return ost;
		return ost << core::astr_wstr(a8str_view.data(), a8str_view.length());
	}

	inline std::wostream & operator<<(std::wostream && ost, const std::string_view & a8str_view)
	{
		if (!ost.good())
			return ost;
		return ost << core::astr_wstr(a8str_view.data(), a8str_view.length());
	}

#ifdef __cpp_char8_t
	inline std::wostream & operator<<(std::wostream & ost, const std::u8string & u8str)
	{
		if (!ost.good())
			return ost;
		return ost << core::u8str_wstr(u8str);
	}

	inline std::wostream & operator<<(std::wostream && ost, const std::u8string & u8str)
	{
		if (!ost.good())
			return ost;
		return ost << core::u8str_wstr(u8str);
	}

	inline std::wostream & operator<<(std::wostream & ost, const std::u8string_view & u8str_view)
	{
		if (!ost.good())
			return ost;
		return ost << core::u8str_wstr(u8str_view.data(), u8str_view.length());
	}

	inline std::wostream & operator<<(std::wostream && ost, const std::u8string_view & u8str_view)
	{
		if (!ost.good())
			return ost;
		return ost << core::u8str_wstr(u8str_view.data(), u8str_view.length());
	}
#endif

	inline std::wostream & operator<<(std::wostream & ost, const std::u16string & u16str)
	{
		if (!ost.good())
			return ost;
		return ost << core::u16str_wstr(u16str);
	}

	inline std::wostream & operator<<(std::wostream && ost, const std::u16string & u16str)
	{
		if (!ost.good())
			return ost;
		return ost << core::u16str_wstr(u16str);
	}


	inline std::wostream & operator<<(std::wostream & ost, const std::u16string_view & u16strview)
	{
		if (!ost.good())
			return ost;
		return ost << core::u16str_wstr(u16strview.data(), u16strview.length());
	}

	inline std::wostream & operator<<(std::wostream && ost, const std::u16string_view & u16strview)
	{
		if (!ost.good())
			return ost;
		return ost << core::u16str_wstr(u16strview.data(), u16strview.length());
	}


	template<typename stream_t>
	void fmt_helper(stream_t & stream) {}

	template<typename stream_t, typename Head, typename ...Tail>
	void fmt_helper(stream_t & stream, const Head & head, Tail && ...tail)
	{
		stream << head;
		return fmt_helper(stream, std::forward<Tail>(tail)...);
	}

	template<typename ...Args>
	std::u8string fmt(Args && ...args)
	{
		std::u8ostringstream stream;
		fmt_helper(stream, std::forward<Args>(args)...);
		return stream.str();
	}

	template<typename ...Args>
	std::string afmt(Args && ...args)
	{
		std::ostringstream stream;
		fmt_helper(stream, std::forward<Args>(args)...);
		return stream.str();
	}

	template<typename ...Args>
	std::wstring wfmt(Args && ...args)
	{
		std::wostringstream stream;
		fmt_helper(stream, std::forward<Args>(args)...);
		return stream.str();
	}
}