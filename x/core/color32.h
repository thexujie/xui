#pragma once

namespace core
{
    struct color32f
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
    class color32
    {
    public:
        color32() = default;
        constexpr color32(uint32_t _argb) : argb(_argb) {}
        constexpr color32(uint8_t a, uint8_t r, uint8_t g, uint8_t b) : a(a), r(r), g(g), b(b) { }

        operator color32f() const
        {
            color32f colorf = { (float32_t)a / 0xFF, (float32_t)r / 0xFF, (float32_t)g / 0xFF, (float32_t)b / 0xFF };
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
        inline static constexpr color32 Auto = 0x00000000;
        //! 自动颜色
        //! 透明
        inline static constexpr color32 Transparent = 0x00FFFFFF;

        //! 红色
        inline static constexpr color32 Red = 0xFFFF0000;
        //! 深红、猩红
        inline static constexpr color32 DarkRed = 0xFFDC143C;
        //! 爱丽丝蓝
        inline static constexpr color32       AliceBlue = 0xFFF0F8FF;
        //! 古董白
        inline static constexpr color32        AntiqueWhite = 0xFFFAEBD7;
        //! 水蓝
        inline static constexpr color32 Aqua = 0xFF00FFFF;
        //! 海蓝、碧绿
        inline static constexpr color32      Aquamarine = 0xFF7FFFD4;
        //! 天蓝
        inline static constexpr color32     Azure = 0xFFF0FFFF;
        //! 米黄
        inline static constexpr color32      Beige = 0xFFF5F5DC;
        //! 橘黄
        inline static constexpr color32    Bisque = 0xFFFFE4C4;
        //! 深黑
        inline static constexpr color32   Black = 0xFF000000;
        //! 杏仁白
        inline static constexpr color32    BlanchedAlmond = 0xFFFFEBCD;
        //! 深蓝
        inline static constexpr color32  Blue = 0xFF0000FF;
        //! 紫罗兰
        inline static constexpr color32  BlueViolet = 0xFF8A2BE2;
        //! 棕色
        inline static constexpr color32   Brown = 0xFFA52A2A;
        //! 实木、原木、硬木
        inline static constexpr color32 BurlyWood = 0xFFDEB887;
        //! 灰蓝色
        inline static constexpr color32 CadetBlue = 0xFF5F9EA0;
        //! 黄绿
        inline static constexpr color32  Chartreuse = 0xFF7FFF00;
        //! 巧克力
        inline static constexpr color32  Chocolate = 0xFFD2691E;
        //! 珊瑚
        inline static constexpr color32  Coral = 0xFFFF7F50;
        //! 浅蓝、矢车菊
        inline static constexpr color32  CornflowerBlue = 0xFF6495ED;
        //! 米绸色
        inline static constexpr color32 Cornsilk = 0xFFFFF8DC;
        //! 猩红色
        inline static constexpr color32  Crimson = 0xFFDC143C;
        //! 青色
        inline static constexpr color32  Cyan = 0xFF00FFFF;
        //! 深蓝色
        inline static constexpr color32 DarkBlue = 0xFF00008B;
        //! 深青色，暗青色，深青绿
        inline static constexpr color32  DarkCyan = 0xFF008B8B;
        inline static constexpr color32  DarkGoldenrod = 0xFFB8860B;
        //! 暗金黄色，暗金菊黄
        inline static constexpr color32 DarkGray = 0xFFA9A9A9;
        //! 深绿色
        inline static constexpr color32   DarkGreen = 0xFF006400;
        //! 深卡其布，暗卡其色，暗黄褐色
        inline static constexpr color32  DarkKhaki = 0xFFBDB76B;
        //! 深洋红
        inline static constexpr color32  DarkMagenta = 0xFF8B008B;
        inline static constexpr color32 DarkOliveGreen = 0xFF556B2F;
        inline static constexpr color32 DarkOrange = 0xFFFF8C00;
        inline static constexpr color32 DarkOrchid = 0xFF9932CC;
        inline static constexpr color32 DarkSalmon = 0xFFE9967A;
        inline static constexpr color32 DarkSeaGreen = 0xFF8FBC8B;
        inline static constexpr color32 DarkSlateBlue = 0xFF483D8B;
        inline static constexpr color32 DarkSlateGray = 0xFF2F4F4F;
        inline static constexpr color32 DarkTurquoise = 0xFF00CED1;
        inline static constexpr color32 DarkViolet = 0xFF9400D3;
        inline static constexpr color32 DeepPink = 0xFFFF1493;
        inline static constexpr color32 DeepSkyBlue = 0xFF00BFFF;
        inline static constexpr color32 DimGray = 0xFF696969;
        inline static constexpr color32 DodgerBlue = 0xFF1E90FF;
        inline static constexpr color32 Firebrick = 0xFFB22222;
        inline static constexpr color32 FloralWhite = 0xFFFFFAF0;
        inline static constexpr color32 ForestGreen = 0xFF228B22;
        inline static constexpr color32 Fuchsia = 0xFFFF00FF;
        inline static constexpr color32 Gainsboro = 0xFFDCDCDC;
        inline static constexpr color32 GhostWhite = 0xFFF8F8FF;
        inline static constexpr color32 Gold = 0xFFFFD700;
        inline static constexpr color32 Goldenrod = 0xFFDAA520;
        inline static constexpr color32 Gray = 0xFF808080;
        inline static constexpr color32 Green = 0xFF008000;
        inline static constexpr color32 GreenYellow = 0xFFADFF2F;
        inline static constexpr color32 Honeydew = 0xFFF0FFF0;
        inline static constexpr color32 HotPink = 0xFFFF69B4;
        inline static constexpr color32 IndianRed = 0xFFCD5C5C;
        inline static constexpr color32 Indigo = 0xFF4B0082;
        inline static constexpr color32 Ivory = 0xFFFFFFF0;
        inline static constexpr color32 Khaki = 0xFFF0E68C;
        inline static constexpr color32 Lavender = 0xFFE6E6FA;
        inline static constexpr color32 LavenderBlush = 0xFFFFF0F5;
        inline static constexpr color32 LawnGreen = 0xFF7CFC00;
        inline static constexpr color32 LemonChiffon = 0xFFFFFACD;
        inline static constexpr color32 LightBlue = 0xFFADD8E6;
        inline static constexpr color32 LightCoral = 0xFFF08080;
        inline static constexpr color32 LightCyan = 0xFFE0FFFF;
        inline static constexpr color32 LightGoldenrodYellow = 0xFFFAFAD2;
        inline static constexpr color32 LightGray = 0xFFD3D3D3;
        inline static constexpr color32 LightGreen = 0xFF90EE90;
        inline static constexpr color32 LightPink = 0xFFFFB6C1;
        inline static constexpr color32 LightSalmon = 0xFFFFA07A;
        inline static constexpr color32 LightSeaGreen = 0xFF20B2AA;
        inline static constexpr color32 LightSkyBlue = 0xFF87CEFA;
        inline static constexpr color32 LightSlateGray = 0xFF778899;
        inline static constexpr color32 LightSteelBlue = 0xFFB0C4DE;
        inline static constexpr color32 LightYellow = 0xFFFFFFE0;
        inline static constexpr color32 Lime = 0xFF00FF00;
        inline static constexpr color32 LimeGreen = 0xFF32CD32;
        inline static constexpr color32 Linen = 0xFFFAF0E6;
        //! 洋红、紫红、品红
        inline static constexpr color32  Magenta = 0xFFFF00FF;
        inline static constexpr color32  Maroon = 0xFF800000;
        inline static constexpr color32  MediumAquamarine = 0xFF66CDAA;
        inline static constexpr color32  MediumBlue = 0xFF0000CD;
        inline static constexpr color32  MediumOrchid = 0xFFBA55D3;
        inline static constexpr color32  MediumPurple = 0xFF9370DB;
        inline static constexpr color32  MediumSeaGreen = 0xFF3CB371;
        inline static constexpr color32  MediumSlateBlue = 0xFF7B68EE;
        inline static constexpr color32  MediumSpringGreen = 0xFF00FA9A;
        inline static constexpr color32  MediumTurquoise = 0xFF48D1CC;
        inline static constexpr color32  MediumVioletRed = 0xFFC71585;
        inline static constexpr color32  MidnightBlue = 0xFF191970;
        inline static constexpr color32  MintCream = 0xFFF5FFFA;
        inline static constexpr color32  MistyRose = 0xFFFFE4E1;
        inline static constexpr color32  Moccasin = 0xFFFFE4B5;
        inline static constexpr color32  NavajoWhite = 0xFFFFDEAD;
        inline static constexpr color32  Navy = 0xFF000080;
        inline static constexpr color32  OldLace = 0xFFFDF5E6;
        inline static constexpr color32  Olive = 0xFF808000;
        inline static constexpr color32  OliveDrab = 0xFF6B8E23;
        inline static constexpr color32  Orange = 0xFFFFA500;
        inline static constexpr color32  OrangeRed = 0xFFFF4500;
        inline static constexpr color32  Orchid = 0xFFDA70D6;
        inline static constexpr color32  PaleGoldenrod = 0xFFEEE8AA;
        inline static constexpr color32  PaleGreen = 0xFF98FB98;
        inline static constexpr color32  PaleTurquoise = 0xFFAFEEEE;
        inline static constexpr color32  PaleVioletRed = 0xFFDB7093;
        inline static constexpr color32  PapayaWhip = 0xFFFFEFD5;
        inline static constexpr color32  PeachPuff = 0xFFFFDAB9;
        inline static constexpr color32  Peru = 0xFFCD853F;
        inline static constexpr color32  Pink = 0xFFFFC0CB;
        inline static constexpr color32  Plum = 0xFFDDA0DD;
        inline static constexpr color32  PowderBlue = 0xFFB0E0E6;
        inline static constexpr color32  Purple = 0xFF800080;
        inline static constexpr color32  RosyBrown = 0xFFBC8F8F;
        inline static constexpr color32  RoyalBlue = 0xFF4169E1;
        inline static constexpr color32  SaddleBrown = 0xFF8B4513;
        inline static constexpr color32  Salmon = 0xFFFA8072;
        inline static constexpr color32  SandyBrown = 0xFFF4A460;
        inline static constexpr color32  SeaGreen = 0xFF2E8B57;
        inline static constexpr color32  SeaShell = 0xFFFFF5EE;
        inline static constexpr color32  Sienna = 0xFFA0522D;
        inline static constexpr color32  Silver = 0xFFC0C0C0;
        inline static constexpr color32  SkyBlue = 0xFF87CEEB;
        inline static constexpr color32  SlateBlue = 0xFF6A5ACD;
        inline static constexpr color32  SlateGray = 0xFF708090;
        //! 雪白
        inline static constexpr color32     Snow = 0xFFFFFAFA;
        //! 春绿色
        inline static constexpr color32     SpringGreen = 0xFF00FF7F;
        inline static constexpr color32     SteelBlue = 0xFF4682B4;
        inline static constexpr color32     Tan = 0xFFD2B48C;
        inline static constexpr color32     Teal = 0xFF008080;
        //! 蓟色
        inline static constexpr color32     Thistle = 0xFFD8BFD8;
        inline static constexpr color32     Tomato = 0xFFFF6347;
        inline static constexpr color32     Turquoise = 0xFF40E0D0;
        inline static constexpr color32     Violet = 0xFFEE82EE;
        inline static constexpr color32     Wheat = 0xFFF5DEB3;
        inline static constexpr color32     White = 0xFFFFFFFF;
        inline static constexpr color32     WhiteSmoke = 0xFFF5F5F5;
        inline static constexpr color32     Yellow = 0xFFFFFF00;
        inline static constexpr color32     YellowGreen = 0xFF9ACD32;
    };
}
