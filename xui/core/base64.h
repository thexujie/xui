#pragma once

namespace core
{
    namespace base64
    {
        const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        const char indices[] =
        {
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            62, // '+'
            -1, -1, -1,
            63, // '/'
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
            -1, -1, -1, -1, -1, -1, -1,
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
            -1, -1, -1, -1, -1, -1,
            26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
            -1, -1, -1, -1, -1
        };

        template<typename _Src, typename _Dst>
        _Dst encode(const _Src & src)
        {
            _Dst dst;
            dst.resize((src.size() + 2) / 3 * 4);

            typename  _Src::size_type isrc = 0;
            typename  _Dst::size_type idst = 0;
            typename  _Src::size_type src_quot = src.size() / 3;
            while (src_quot--)
            {
                uint8_t ch0 = static_cast<uint8_t>(src[isrc++]);
                uint8_t ch1 = static_cast<uint8_t>(src[isrc++]);
                uint8_t ch2 = static_cast<uint8_t>(src[isrc++]);

                uint8_t idx0 = ch0 >> 2;
                uint8_t idx1 = ((ch0 << 4) & 0b00110000) | (ch1 >> 4);
                uint8_t idx2 = ((ch1 & 0b1111) << 2) | (ch2 >> 6);
                uint8_t idx3 = ch2 & 0b111111;

                dst[idst++] = chars[idx0];
                dst[idst++] = chars[idx1];
                dst[idst++] = chars[idx2];
                dst[idst++] = chars[idx3];
            }

            typename  _Src::size_type src_odd = src.size() / 3;
            if (src_odd > 0)
            {
                uint8_t ch0 = static_cast<uint8_t>(src[isrc++]);
                uint8_t idx0 = ch0 >> 2;
                dst[idst++] = chars[idx0];
                if (src_odd > 1)
                {
                    uint8_t ch1 = static_cast<uint8_t>(src[isrc++]);
                    uint8_t idx1 = ((ch0 << 4) & 0b00110000) | (ch1 >> 4);
                    uint8_t idx2 = (ch1 & 0b1111) << 2;

                    dst[idst++] = chars[idx1];
                    dst[idst++] = chars[idx2];
                    dst[idst++] = '=';
                }
                else
                {
                    uint8_t idx1 = (ch0 << 4) & 0b00110000;
                    dst[idst++] = chars[idx1];
                    dst[idst++] = '=';
                    dst[idst++] = '=';
                }
            }

            return dst;
        }

        inline std::string encode(const std::string & src)
        {
            return encode<std::string, std::string>(src);
        }

        inline std::vector<uint8_t> base64_encode_s2b(const std::string & src)
        {
            return encode<std::string, std::vector<uint8_t>>(src);
        }

        inline std::string base64_encode_b2s(const std::vector<uint8_t> & src)
        {
            return encode<std::vector<uint8_t>, std::string>(src);
        }

        template<typename _Src, typename _Dst>
        _Dst decode(const _Src & src)
        {
            if (src.size() < 4 || src.size() % 4 != 0)
                return {};

            _Dst dst;
            dst.resize((src.size() + 3) / 4 * 3);

            typename  _Src::size_type isrc = 0;
            typename  _Dst::size_type idst = 0;
            typename  _Src::size_type src_patch = src.size() / 4;

            while (src_patch--)
            {
                uint8_t ch0 = src[isrc++];
                uint8_t ch1 = src[isrc++];
                uint8_t ch2 = src[isrc++];
                uint8_t ch3 = src[isrc++];

                uint8_t idx0 = ch0 > 127 ? -1 : indices[ch0];
                uint8_t idx1 = ch1 > 127 ? -1 : indices[ch1];

                if (idx0 < 0 || idx0 > 127 ||
                    idx1 < 0 || idx1 > 127)
                    return {};

                dst[idst++] = (idx0 << 2) | (idx1 >> 4);
                if (ch2 != '=')
                {
                    uint8_t idx2 = ch2 > 127 ? -1 : indices[ch2];
                    if (idx2 < 0 || idx2 > 127)
                        return {};
                    dst[idst++] = ((idx1 << 4) & 0xf0) | (idx2 >> 2);
                    if (ch3 != '=')
                    {
                        uint8_t idx3 = ch3 > 127 ? -1 : indices[ch3];
                        if (idx3 < 0 || idx3 > 127)
                            return {};
                        dst[idst++] = ((idx2 << 6) & 0xc0) | idx3;
                    }
                }
            }
            dst.resize(idst);
            return dst;
        }

        inline std::string decode(const std::string & src)
        {
            return decode<std::string, std::string>(src);
        }

        inline std::vector<char> decode_s2cs(const std::string & src)
        {
            return decode<std::string, std::vector<char>>(src);
        }

        inline std::vector<uint8_t> decode_s2bs(const std::string & src)
        {
            return decode<std::string, std::vector<uint8_t>>(src);
        }

        inline std::string base64_decode_b2s(const std::vector<uint8_t> & src)
        {
            return decode<std::vector<uint8_t>, std::string>(src);
        }
    }
}
