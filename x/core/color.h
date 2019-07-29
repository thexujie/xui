#pragma once

namespace core
{
    struct colorf
    {
        union
        {
            struct
            {
                float32_t a;
                float32_t r;
                float32_t g;
                float32_t b;
            };

            float32_t arr[4];
        };
    };

    // 整数形式ARGB颜色
    class color
    {
    public:
        color() = default;
        constexpr color(uint32_t _argb) : argb(_argb) {}
        constexpr color(uint8_t a, uint8_t r, uint8_t g, uint8_t b) : a(a), r(r), g(g), b(b) { }

        color operator * (float32_t rate) const
        {
            if (a == 0)
                return color(0, static_cast<uint8_t>(r * rate), static_cast<uint8_t>(g * rate), static_cast<uint8_t>(b * rate));

            if (a == 0xff)
                return color(0xff, static_cast<uint8_t>(r * rate), static_cast<uint8_t>(g * rate), static_cast<uint8_t>(b * rate));

            return color(static_cast<uint8_t>(a * rate), r, g, b);
        }

        operator colorf() const
        {
            colorf colorf = { (float32_t)a / 0xFF, (float32_t)r / 0xFF, (float32_t)g / 0xFF, (float32_t)b / 0xFF };
            return colorf;
        }

        operator uint32_t() const
        {
            return argb;
        }

        bool visible() const { return a > 0; }

        union
        {
            uint32_t argb;

            struct
            {
                uint8_t b;
                uint8_t g;
                uint8_t r;
                uint8_t a;
            };
        };
    };

    class colors
    {
    public:
        inline static constexpr color Auto = 0x00000000;
        //! 自动颜色
        //! 透明
        inline static constexpr color Transparent = 0x00FFFFFF;

        //! 红色
        inline static constexpr color Red = 0xFFFF0000;
        //! 深红、猩红
        inline static constexpr color DarkRed = 0xFFDC143C;
        //! 爱丽丝蓝
        inline static constexpr color AliceBlue = 0xFFF0F8FF;
        //! 古董白
        inline static constexpr color AntiqueWhite = 0xFFFAEBD7;
        //! 水蓝
        inline static constexpr color Aqua = 0xFF00FFFF;
        //! 海蓝、碧绿
        inline static constexpr color Aquamarine = 0xFF7FFFD4;
        //! 天蓝
        inline static constexpr color Azure = 0xFFF0FFFF;
        //! 米黄
        inline static constexpr color Beige = 0xFFF5F5DC;
        //! 橘黄
        inline static constexpr color Bisque = 0xFFFFE4C4;
        //! 深黑
        inline static constexpr color Black = 0xFF000000;
        //! 杏仁白
        inline static constexpr color BlanchedAlmond = 0xFFFFEBCD;
        //! 深蓝
        inline static constexpr color Blue = 0xFF0000FF;
        //! 紫罗兰
        inline static constexpr color BlueViolet = 0xFF8A2BE2;
        //! 棕色
        inline static constexpr color Brown = 0xFFA52A2A;
        //! 实木、原木、硬木
        inline static constexpr color BurlyWood = 0xFFDEB887;
        //! 灰蓝色
        inline static constexpr color CadetBlue = 0xFF5F9EA0;
        //! 黄绿
        inline static constexpr color Chartreuse = 0xFF7FFF00;
        //! 巧克力
        inline static constexpr color Chocolate = 0xFFD2691E;
        //! 珊瑚
        inline static constexpr color Coral = 0xFFFF7F50;
        //! 浅蓝、矢车菊
        inline static constexpr color CornflowerBlue = 0xFF6495ED;
        //! 米绸色
        inline static constexpr color Cornsilk = 0xFFFFF8DC;
        //! 猩红色
        inline static constexpr color Crimson = 0xFFDC143C;
        //! 青色
        inline static constexpr color Cyan = 0xFF00FFFF;
        //! 深蓝色
        inline static constexpr color DarkBlue = 0xFF00008B;
        //! 深青色，暗青色，深青绿
        inline static constexpr color DarkCyan = 0xFF008B8B;
        inline static constexpr color DarkGoldenrod = 0xFFB8860B;
        //! 暗金黄色，暗金菊黄
        inline static constexpr color DarkGray = 0xFFA9A9A9;
        //! 深绿色
        inline static constexpr color DarkGreen = 0xFF006400;
        //! 深卡其布，暗卡其色，暗黄褐色
        inline static constexpr color DarkKhaki = 0xFFBDB76B;
        //! 深洋红
        inline static constexpr color DarkMagenta = 0xFF8B008B;
        inline static constexpr color DarkOliveGreen = 0xFF556B2F;
        inline static constexpr color DarkOrange = 0xFFFF8C00;
        inline static constexpr color DarkOrchid = 0xFF9932CC;
        inline static constexpr color DarkSalmon = 0xFFE9967A;
        inline static constexpr color DarkSeaGreen = 0xFF8FBC8B;
        inline static constexpr color DarkSlateBlue = 0xFF483D8B;
        inline static constexpr color DarkSlateGray = 0xFF2F4F4F;
        inline static constexpr color DarkTurquoise = 0xFF00CED1;
        inline static constexpr color DarkViolet = 0xFF9400D3;
        inline static constexpr color DeepPink = 0xFFFF1493;
        inline static constexpr color DeepSkyBlue = 0xFF00BFFF;
        inline static constexpr color DimGray = 0xFF696969;
        inline static constexpr color DodgerBlue = 0xFF1E90FF;
        inline static constexpr color Firebrick = 0xFFB22222;
        inline static constexpr color FloralWhite = 0xFFFFFAF0;
        inline static constexpr color ForestGreen = 0xFF228B22;
        inline static constexpr color Fuchsia = 0xFFFF00FF;
        inline static constexpr color Gainsboro = 0xFFDCDCDC;
        inline static constexpr color GhostWhite = 0xFFF8F8FF;
        inline static constexpr color Gold = 0xFFFFD700;
        inline static constexpr color Goldenrod = 0xFFDAA520;
        inline static constexpr color Gray = 0xFF808080;
        inline static constexpr color Green = 0xFF008000;
        inline static constexpr color GreenYellow = 0xFFADFF2F;
        inline static constexpr color Honeydew = 0xFFF0FFF0;
        inline static constexpr color HotPink = 0xFFFF69B4;
        inline static constexpr color IndianRed = 0xFFCD5C5C;
        inline static constexpr color Indigo = 0xFF4B0082;
        inline static constexpr color Ivory = 0xFFFFFFF0;
        inline static constexpr color Khaki = 0xFFF0E68C;
        inline static constexpr color Lavender = 0xFFE6E6FA;
        inline static constexpr color LavenderBlush = 0xFFFFF0F5;
        inline static constexpr color LawnGreen = 0xFF7CFC00;
        inline static constexpr color LemonChiffon = 0xFFFFFACD;
        inline static constexpr color LightBlue = 0xFFADD8E6;
        inline static constexpr color LightCoral = 0xFFF08080;
        inline static constexpr color LightCyan = 0xFFE0FFFF;
        inline static constexpr color LightGoldenrodYellow = 0xFFFAFAD2;
        inline static constexpr color LightGray = 0xFFD3D3D3;
        inline static constexpr color LightGreen = 0xFF90EE90;
        inline static constexpr color LightPink = 0xFFFFB6C1;
        inline static constexpr color LightSalmon = 0xFFFFA07A;
        inline static constexpr color LightSeaGreen = 0xFF20B2AA;
        inline static constexpr color LightSkyBlue = 0xFF87CEFA;
        inline static constexpr color LightSlateGray = 0xFF778899;
        inline static constexpr color LightSteelBlue = 0xFFB0C4DE;
        inline static constexpr color LightYellow = 0xFFFFFFE0;
        inline static constexpr color Lime = 0xFF00FF00;
        inline static constexpr color LimeGreen = 0xFF32CD32;
        inline static constexpr color Linen = 0xFFFAF0E6;
        //! 洋红、紫红、品红
        inline static constexpr color Magenta = 0xFFFF00FF;
        inline static constexpr color Maroon = 0xFF800000;
        inline static constexpr color MediumAquamarine = 0xFF66CDAA;
        inline static constexpr color MediumBlue = 0xFF0000CD;
        inline static constexpr color MediumOrchid = 0xFFBA55D3;
        inline static constexpr color MediumPurple = 0xFF9370DB;
        inline static constexpr color MediumSeaGreen = 0xFF3CB371;
        inline static constexpr color MediumSlateBlue = 0xFF7B68EE;
        inline static constexpr color MediumSpringGreen = 0xFF00FA9A;
        inline static constexpr color MediumTurquoise = 0xFF48D1CC;
        inline static constexpr color MediumVioletRed = 0xFFC71585;
        inline static constexpr color MidnightBlue = 0xFF191970;
        inline static constexpr color MintCream = 0xFFF5FFFA;
        inline static constexpr color MistyRose = 0xFFFFE4E1;
        inline static constexpr color Moccasin = 0xFFFFE4B5;
        inline static constexpr color NavajoWhite = 0xFFFFDEAD;
        inline static constexpr color Navy = 0xFF000080;
        inline static constexpr color OldLace = 0xFFFDF5E6;
        inline static constexpr color Olive = 0xFF808000;
        inline static constexpr color OliveDrab = 0xFF6B8E23;
        inline static constexpr color Orange = 0xFFFFA500;
        inline static constexpr color OrangeRed = 0xFFFF4500;
        inline static constexpr color Orchid = 0xFFDA70D6;
        inline static constexpr color PaleGoldenrod = 0xFFEEE8AA;
        inline static constexpr color PaleGreen = 0xFF98FB98;
        inline static constexpr color PaleTurquoise = 0xFFAFEEEE;
        inline static constexpr color PaleVioletRed = 0xFFDB7093;
        inline static constexpr color PapayaWhip = 0xFFFFEFD5;
        inline static constexpr color PeachPuff = 0xFFFFDAB9;
        inline static constexpr color Peru = 0xFFCD853F;
        inline static constexpr color Pink = 0xFFFFC0CB;
        inline static constexpr color Plum = 0xFFDDA0DD;
        inline static constexpr color PowderBlue = 0xFFB0E0E6;
        inline static constexpr color Purple = 0xFF800080;
        inline static constexpr color RosyBrown = 0xFFBC8F8F;
        inline static constexpr color RoyalBlue = 0xFF4169E1;
        inline static constexpr color SaddleBrown = 0xFF8B4513;
        inline static constexpr color Salmon = 0xFFFA8072;
        inline static constexpr color SandyBrown = 0xFFF4A460;
        inline static constexpr color SeaGreen = 0xFF2E8B57;
        inline static constexpr color SeaShell = 0xFFFFF5EE;
        inline static constexpr color Sienna = 0xFFA0522D;
        inline static constexpr color Silver = 0xFFC0C0C0;
        inline static constexpr color SkyBlue = 0xFF87CEEB;
        inline static constexpr color SlateBlue = 0xFF6A5ACD;
        inline static constexpr color SlateGray = 0xFF708090;
        //! 雪白
        inline static constexpr color Snow = 0xFFFFFAFA;
        //! 春绿色
        inline static constexpr color SpringGreen = 0xFF00FF7F;
        inline static constexpr color SteelBlue = 0xFF4682B4;
        inline static constexpr color Tan = 0xFFD2B48C;
        inline static constexpr color Teal = 0xFF008080;
        //! 蓟色
        inline static constexpr color Thistle = 0xFFD8BFD8;
        inline static constexpr color Tomato = 0xFFFF6347;
        inline static constexpr color Turquoise = 0xFF40E0D0;
        inline static constexpr color Violet = 0xFFEE82EE;
        inline static constexpr color Wheat = 0xFFF5DEB3;
        inline static constexpr color White = 0xFFFFFFFF;
        inline static constexpr color WhiteSmoke = 0xFFF5F5F5;
        inline static constexpr color Yellow = 0xFFFFFF00;
        inline static constexpr color YellowGreen = 0xFF9ACD32;
    };

    struct color_name
    {
        color color;
        const char8_t * name = nullptr;
    };

    const color_name color_names[] =
    {
        { colors::Auto, u8"Auto" },
        { colors::Transparent, u8"Transparent" },
        { colors::Red, u8"Red" },
        { colors::DarkRed, u8"DarkRed" },
        { colors::Aqua, u8"Aqua" },
        { colors::Bisque, u8"Bisque" },
        { colors::Black, u8"Black" },
        { colors::BlanchedAlmond, u8"BlanchedAlmond" },
        { colors::Blue, u8"Blue" },
        { colors::BlueViolet, u8"BlueViolet" },
        { colors::Brown, u8"Brown" },
        { colors::BurlyWood, u8"BurlyWood" },
        { colors::CadetBlue, u8"CadetBlue" },
        { colors::Chartreuse, u8"Chartreuse" },
        { colors::Chocolate, u8"Chocolate" },
        { colors::Coral, u8"Coral" },
        { colors::CornflowerBlue, u8"CornflowerBlue" },
        { colors::Cornsilk, u8"Cornsilk" },
        { colors::Crimson, u8"Crimson" },
        { colors::Cyan, u8"Cyan" },
        { colors::DarkBlue, u8"DarkBlue" },
        { colors::DarkCyan, u8"DarkCyan" },
        { colors::DarkGoldenrod, u8"DarkGoldenrod" },
        { colors::DarkGray, u8"DarkGray" },
        { colors::DarkGreen, u8"DarkGreen" },
        { colors::DarkKhaki, u8"DarkKhaki" },
        { colors::DarkMagenta, u8"DarkMagenta" },
        { colors::DarkOliveGreen, u8"DarkOliveGreen" },
        { colors::DarkOrange, u8"DarkOrange" },
        { colors::DarkOrchid, u8"DarkOrchid" },
        { colors::DarkSalmon, u8"DarkSalmon" },
        { colors::DarkSeaGreen, u8"DarkSeaGreen" },
        { colors::DarkSlateBlue, u8"DarkSlateBlue" },
        { colors::DarkSlateGray, u8"DarkSlateGray" },
        { colors::DarkTurquoise, u8"DarkTurquoise" },
        { colors::DarkViolet, u8"DarkViolet" },
        { colors::DeepPink, u8"DeepPink" },
        { colors::DeepSkyBlue, u8"DeepSkyBlue" },
        { colors::DimGray, u8"DimGray" },
        { colors::DodgerBlue, u8"DodgerBlue" },
        { colors::Firebrick, u8"Firebrick" },
        { colors::FloralWhite, u8"FloralWhite" },
        { colors::ForestGreen, u8"ForestGreen" },
        { colors::Fuchsia, u8"Fuchsia" },
        { colors::Gainsboro, u8"Gainsboro" },
        { colors::GhostWhite, u8"GhostWhite" },
        { colors::Gold, u8"Gold" },
        { colors::Goldenrod, u8"Goldenrod" },
        { colors::Gray, u8"Gray" },
        { colors::Green, u8"Green" },
        { colors::GreenYellow, u8"GreenYellow" },
        { colors::Honeydew, u8"Honeydew" },
        { colors::HotPink, u8"HotPink" },
        { colors::IndianRed, u8"IndianRed" },
        { colors::Indigo, u8"Indigo" },
        { colors::Ivory, u8"Ivory" },
        { colors::Khaki, u8"Khaki" },
        { colors::Lavender, u8"Lavender" },
        { colors::LavenderBlush, u8"LavenderBlush" },
        { colors::LawnGreen, u8"LawnGreen" },
        { colors::LemonChiffon, u8"LemonChiffon" },
        { colors::LightBlue, u8"LightBlue" },
        { colors::LightCoral, u8"LightCoral" },
        { colors::LightCyan, u8"LightCyan" },
        { colors::LightGoldenrodYellow, u8"LightGoldenrodYell" },
        { colors::LightGray, u8"LightGray" },
        { colors::LightGreen, u8"LightGreen" },
        { colors::LightPink, u8"LightPink" },
        { colors::LightSalmon, u8"LightSalmon" },
        { colors::LightSeaGreen, u8"LightSeaGreen" },
        { colors::LightSkyBlue, u8"LightSkyBlue" },
        { colors::LightSlateGray, u8"LightSlateGray" },
        { colors::LightSteelBlue, u8"LightSteelBlue" },
        { colors::LightYellow, u8"LightYellow" },
        { colors::Lime, u8"Lime" },
        { colors::LimeGreen, u8"LimeGreen" },
        { colors::Linen, u8"Linen" },
        { colors::Magenta, u8"Magenta" },
        { colors::Maroon, u8"Maroon" },
        { colors::MediumAquamarine, u8"MediumAquamarine" },
        { colors::MediumBlue, u8"MediumBlue" },
        { colors::MediumOrchid, u8"MediumOrchid" },
        { colors::MediumPurple, u8"MediumPurple" },
        { colors::MediumSeaGreen, u8"MediumSeaGreen" },
        { colors::MediumSlateBlue, u8"MediumSlateBlue" },
        { colors::MediumSpringGreen, u8"MediumSpringGreen" },
        { colors::MediumTurquoise, u8"MediumTurquoise" },
        { colors::MediumVioletRed, u8"MediumVioletRed" },
        { colors::MidnightBlue, u8"MidnightBlue" },
        { colors::MintCream, u8"MintCream" },
        { colors::MistyRose, u8"MistyRose" },
        { colors::Moccasin, u8"Moccasin" },
        { colors::NavajoWhite, u8"NavajoWhite" },
        { colors::Navy, u8"Navy" },
        { colors::OldLace, u8"OldLace" },
        { colors::Olive, u8"Olive" },
        { colors::OliveDrab, u8"OliveDrab" },
        { colors::Orange, u8"Orange" },
        { colors::OrangeRed, u8"OrangeRed" },
        { colors::Orchid, u8"Orchid" },
        { colors::PaleGoldenrod, u8"PaleGoldenrod" },
        { colors::PaleGreen, u8"PaleGreen" },
        { colors::PaleTurquoise, u8"PaleTurquoise" },
        { colors::PaleVioletRed, u8"PaleVioletRed" },
        { colors::PapayaWhip, u8"PapayaWhip" },
        { colors::PeachPuff, u8"PeachPuff" },
        { colors::Peru, u8"Peru" },
        { colors::Pink, u8"Pink" },
        { colors::Plum, u8"Plum" },
        { colors::PowderBlue, u8"PowderBlue" },
        { colors::Purple, u8"Purple" },
        { colors::RosyBrown, u8"RosyBrown" },
        { colors::RoyalBlue, u8"RoyalBlue" },
        { colors::SaddleBrown, u8"SaddleBrown" },
        { colors::Salmon, u8"Salmon" },
        { colors::SandyBrown, u8"SandyBrown" },
        { colors::SeaGreen, u8"SeaGreen" },
        { colors::SeaShell, u8"SeaShell" },
        { colors::Sienna, u8"Sienna" },
        { colors::Silver, u8"Silver" },
        { colors::SkyBlue, u8"SkyBlue" },
        { colors::SlateBlue, u8"SlateBlue" },
        { colors::SlateGray, u8"SlateGray" },
        { colors::Snow, u8"Snow" },
        { colors::SpringGreen, u8"SpringGreen" },
        { colors::SteelBlue, u8"SteelBlue" },
        { colors::Tan, u8"Tan" },
        { colors::Teal, u8"Teal" },
        { colors::Thistle, u8"Thistle" },
        { colors::Tomato, u8"Tomato" },
        { colors::Turquoise, u8"Turquoise" },
        { colors::Violet, u8"Violet" },
        { colors::Wheat, u8"Wheat" },
        { colors::White, u8"White" },
        { colors::WhiteSmoke, u8"WhiteSmoke" },
        { colors::Yellow, u8"Yellow" },
        { colors::YellowGreen, u8"YellowGreen" },
    };
}
