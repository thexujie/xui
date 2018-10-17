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
        const char * name = nullptr;
    };

    const color_name color_names[] =
    {
        { colors::Auto, "Auto" },
        { colors::Transparent, "Transparent" },
        { colors::Red, "Red" },
        { colors::DarkRed, "DarkRed" },
        { colors::Aqua, "Aqua" },
        { colors::Bisque, "Bisque" },
        { colors::Black, "Black" },
        { colors::BlanchedAlmond, "BlanchedAlmond" },
        { colors::Blue, "Blue" },
        { colors::BlueViolet, "BlueViolet" },
        { colors::Brown, "Brown" },
        { colors::BurlyWood, "BurlyWood" },
        { colors::CadetBlue, "CadetBlue" },
        { colors::Chartreuse, "Chartreuse" },
        { colors::Chocolate, "Chocolate" },
        { colors::Coral, "Coral" },
        { colors::CornflowerBlue, "CornflowerBlue" },
        { colors::Cornsilk, "Cornsilk" },
        { colors::Crimson, "Crimson" },
        { colors::Cyan, "Cyan" },
        { colors::DarkBlue, "DarkBlue" },
        { colors::DarkCyan, "DarkCyan" },
        { colors::DarkGoldenrod, "DarkGoldenrod" },
        { colors::DarkGray, "DarkGray" },
        { colors::DarkGreen, "DarkGreen" },
        { colors::DarkKhaki, "DarkKhaki" },
        { colors::DarkMagenta, "DarkMagenta" },
        { colors::DarkOliveGreen, "DarkOliveGreen" },
        { colors::DarkOrange, "DarkOrange" },
        { colors::DarkOrchid, "DarkOrchid" },
        { colors::DarkSalmon, "DarkSalmon" },
        { colors::DarkSeaGreen, "DarkSeaGreen" },
        { colors::DarkSlateBlue, "DarkSlateBlue" },
        { colors::DarkSlateGray, "DarkSlateGray" },
        { colors::DarkTurquoise, "DarkTurquoise" },
        { colors::DarkViolet, "DarkViolet" },
        { colors::DeepPink, "DeepPink" },
        { colors::DeepSkyBlue, "DeepSkyBlue" },
        { colors::DimGray, "DimGray" },
        { colors::DodgerBlue, "DodgerBlue" },
        { colors::Firebrick, "Firebrick" },
        { colors::FloralWhite, "FloralWhite" },
        { colors::ForestGreen, "ForestGreen" },
        { colors::Fuchsia, "Fuchsia" },
        { colors::Gainsboro, "Gainsboro" },
        { colors::GhostWhite, "GhostWhite" },
        { colors::Gold, "Gold" },
        { colors::Goldenrod, "Goldenrod" },
        { colors::Gray, "Gray" },
        { colors::Green, "Green" },
        { colors::GreenYellow, "GreenYellow" },
        { colors::Honeydew, "Honeydew" },
        { colors::HotPink, "HotPink" },
        { colors::IndianRed, "IndianRed" },
        { colors::Indigo, "Indigo" },
        { colors::Ivory, "Ivory" },
        { colors::Khaki, "Khaki" },
        { colors::Lavender, "Lavender" },
        { colors::LavenderBlush, "LavenderBlush" },
        { colors::LawnGreen, "LawnGreen" },
        { colors::LemonChiffon, "LemonChiffon" },
        { colors::LightBlue, "LightBlue" },
        { colors::LightCoral, "LightCoral" },
        { colors::LightCyan, "LightCyan" },
        { colors::LightGoldenrodYellow, "LightGoldenrodYell" },
        { colors::LightGray, "LightGray" },
        { colors::LightGreen, "LightGreen" },
        { colors::LightPink, "LightPink" },
        { colors::LightSalmon, "LightSalmon" },
        { colors::LightSeaGreen, "LightSeaGreen" },
        { colors::LightSkyBlue, "LightSkyBlue" },
        { colors::LightSlateGray, "LightSlateGray" },
        { colors::LightSteelBlue, "LightSteelBlue" },
        { colors::LightYellow, "LightYellow" },
        { colors::Lime, "Lime" },
        { colors::LimeGreen, "LimeGreen" },
        { colors::Linen, "Linen" },
        { colors::Magenta, "Magenta" },
        { colors::Maroon, "Maroon" },
        { colors::MediumAquamarine, "MediumAquamarine" },
        { colors::MediumBlue, "MediumBlue" },
        { colors::MediumOrchid, "MediumOrchid" },
        { colors::MediumPurple, "MediumPurple" },
        { colors::MediumSeaGreen, "MediumSeaGreen" },
        { colors::MediumSlateBlue, "MediumSlateBlue" },
        { colors::MediumSpringGreen, "MediumSpringGreen" },
        { colors::MediumTurquoise, "MediumTurquoise" },
        { colors::MediumVioletRed, "MediumVioletRed" },
        { colors::MidnightBlue, "MidnightBlue" },
        { colors::MintCream, "MintCream" },
        { colors::MistyRose, "MistyRose" },
        { colors::Moccasin, "Moccasin" },
        { colors::NavajoWhite, "NavajoWhite" },
        { colors::Navy, "Navy" },
        { colors::OldLace, "OldLace" },
        { colors::Olive, "Olive" },
        { colors::OliveDrab, "OliveDrab" },
        { colors::Orange, "Orange" },
        { colors::OrangeRed, "OrangeRed" },
        { colors::Orchid, "Orchid" },
        { colors::PaleGoldenrod, "PaleGoldenrod" },
        { colors::PaleGreen, "PaleGreen" },
        { colors::PaleTurquoise, "PaleTurquoise" },
        { colors::PaleVioletRed, "PaleVioletRed" },
        { colors::PapayaWhip, "PapayaWhip" },
        { colors::PeachPuff, "PeachPuff" },
        { colors::Peru, "Peru" },
        { colors::Pink, "Pink" },
        { colors::Plum, "Plum" },
        { colors::PowderBlue, "PowderBlue" },
        { colors::Purple, "Purple" },
        { colors::RosyBrown, "RosyBrown" },
        { colors::RoyalBlue, "RoyalBlue" },
        { colors::SaddleBrown, "SaddleBrown" },
        { colors::Salmon, "Salmon" },
        { colors::SandyBrown, "SandyBrown" },
        { colors::SeaGreen, "SeaGreen" },
        { colors::SeaShell, "SeaShell" },
        { colors::Sienna, "Sienna" },
        { colors::Silver, "Silver" },
        { colors::SkyBlue, "SkyBlue" },
        { colors::SlateBlue, "SlateBlue" },
        { colors::SlateGray, "SlateGray" },
        { colors::Snow, "Snow" },
        { colors::SpringGreen, "SpringGreen" },
        { colors::SteelBlue, "SteelBlue" },
        { colors::Tan, "Tan" },
        { colors::Teal, "Teal" },
        { colors::Thistle, "Thistle" },
        { colors::Tomato, "Tomato" },
        { colors::Turquoise, "Turquoise" },
        { colors::Violet, "Violet" },
        { colors::Wheat, "Wheat" },
        { colors::White, "White" },
        { colors::WhiteSmoke, "WhiteSmoke" },
        { colors::Yellow, "Yellow" },
        { colors::YellowGreen, "YellowGreen" },
    };
}
