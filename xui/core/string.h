#pragma once

namespace core
{
    namespace string
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
        std::string ucs2_u8(const char16_t * text, int32_t length);
        std::string ucs2_ansi(const char16_t * text, int32_t length);
        std::wstring u8_ucs2(const char8_t * text, int32_t length);
        std::wstring ansi_ucs2(const char8_t * text, int32_t length);

        std::string ansi_u8(std::string str);
        std::string u8_ansi(std::string str);
        std::string usc2_u8(std::wstring str);
        std::string usc2_ansi(std::wstring str);
        std::wstring u8_ucs2(std::string str);
        std::string ucs2_u8(std::wstring str);

        struct less_ic
        {
            bool operator() (const std::string & lhs, const std::string & rhs) const
            {
                std::string str1(lhs.length(), ' ');
                std::string str2(rhs.length(), ' ');
                std::transform(lhs.begin(), lhs.end(), str1.begin(), tolower);
                std::transform(rhs.begin(), rhs.end(), str2.begin(), tolower);
                return  str1 < str2;
            }
        };
        struct equal_ic
        {
            bool operator() (const std::string& s1, const std::string& s2) const
            {
                if (s1.length() != s2.length())
                    return false;

                std::string str1(s1.length(), ' ');
                std::string str2(s2.length(), ' ');
                std::transform(s1.begin(), s1.end(), str1.begin(), tolower);
                std::transform(s2.begin(), s2.end(), str2.begin(), tolower);
                return  str1 == str2;
            }
        };

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
}
