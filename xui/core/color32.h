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
        color32(uint32_t _argb) : argb(_argb) {}
        color32(uint8_t a, uint8_t r, uint8_t g, uint8_t b) : a(a), r(r), g(g), b(b) { }

        operator color32f() const
        {
            color32f colorf = { (float32_t)a / 0xFF, (float32_t)r / 0xFF, (float32_t)g / 0xFF, (float32_t)b / 0xFF };
            return colorf;
        }

        operator uint32_t () const
        {
            return argb;
        }

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
        enum
        {
            //! 自动颜色
            Auto = 0x00000000,
            //! 透明
            Transparent = 0x00FFFFFF,

            //! 红色
            Red = 0xFFFF0000,
            //! 深红、猩红
            DarkRed = 0xFFDC143C,
            //! 爱丽丝蓝
            AliceBlue = 0xFFF0F8FF,
            //! 古董白
            AntiqueWhite = 0xFFFAEBD7,
            //! 水蓝
            Aqua = 0xFF00FFFF,
            //! 海蓝、碧绿
            Aquamarine = 0xFF7FFFD4,
            //! 天蓝
            Azure = 0xFFF0FFFF,
            //! 米黄
            Beige = 0xFFF5F5DC,
            //! 橘黄
            Bisque = 0xFFFFE4C4,
            //! 深黑
            Black = 0xFF000000,
            //! 杏仁白
            BlanchedAlmond = 0xFFFFEBCD,
            //! 深蓝
            Blue = 0xFF0000FF,
            //! 紫罗兰
            BlueViolet = 0xFF8A2BE2,
            //! 棕色
            Brown = 0xFFA52A2A,
            //! 实木、原木、硬木
            BurlyWood = 0xFFDEB887,
            //! 灰蓝色
            CadetBlue = 0xFF5F9EA0,
            //! 黄绿
            Chartreuse = 0xFF7FFF00,
            //! 巧克力
            Chocolate = 0xFFD2691E,
            //! 珊瑚
            Coral = 0xFFFF7F50,
            //! 浅蓝、矢车菊
            CornflowerBlue = 0xFF6495ED,
            //! 米绸色
            Cornsilk = 0xFFFFF8DC,
            //! 猩红色
            Crimson = 0xFFDC143C,
            //! 青色
            Cyan = 0xFF00FFFF,
            //! 深蓝色
            DarkBlue = 0xFF00008B,
            //! 深青色，暗青色，深青绿
            DarkCyan = 0xFF008B8B,
            DarkGoldenrod = 0xFFB8860B,
            //! 暗金黄色，暗金菊黄
            DarkGray = 0xFFA9A9A9,
            //! 深绿色
            DarkGreen = 0xFF006400,
            //! 深卡其布，暗卡其色，暗黄褐色
            DarkKhaki = 0xFFBDB76B,
            //! 深洋红
            DarkMagenta = 0xFF8B008B,
            DarkOliveGreen = 0xFF556B2F,
            DarkOrange = 0xFFFF8C00,
            DarkOrchid = 0xFF9932CC,
            DarkSalmon = 0xFFE9967A,
            DarkSeaGreen = 0xFF8FBC8B,
            DarkSlateBlue = 0xFF483D8B,
            DarkSlateGray = 0xFF2F4F4F,
            DarkTurquoise = 0xFF00CED1,
            DarkViolet = 0xFF9400D3,
            DeepPink = 0xFFFF1493,
            DeepSkyBlue = 0xFF00BFFF,
            DimGray = 0xFF696969,
            DodgerBlue = 0xFF1E90FF,
            Firebrick = 0xFFB22222,
            FloralWhite = 0xFFFFFAF0,
            ForestGreen = 0xFF228B22,
            Fuchsia = 0xFFFF00FF,
            Gainsboro = 0xFFDCDCDC,
            GhostWhite = 0xFFF8F8FF,
            Gold = 0xFFFFD700,
            Goldenrod = 0xFFDAA520,
            Gray = 0xFF808080,
            Green = 0xFF008000,
            GreenYellow = 0xFFADFF2F,
            Honeydew = 0xFFF0FFF0,
            HotPink = 0xFFFF69B4,
            IndianRed = 0xFFCD5C5C,
            Indigo = 0xFF4B0082,
            Ivory = 0xFFFFFFF0,
            Khaki = 0xFFF0E68C,
            Lavender = 0xFFE6E6FA,
            LavenderBlush = 0xFFFFF0F5,
            LawnGreen = 0xFF7CFC00,
            LemonChiffon = 0xFFFFFACD,
            LightBlue = 0xFFADD8E6,
            LightCoral = 0xFFF08080,
            LightCyan = 0xFFE0FFFF,
            LightGoldenrodYellow = 0xFFFAFAD2,
            LightGray = 0xFFD3D3D3,
            LightGreen = 0xFF90EE90,
            LightPink = 0xFFFFB6C1,
            LightSalmon = 0xFFFFA07A,
            LightSeaGreen = 0xFF20B2AA,
            LightSkyBlue = 0xFF87CEFA,
            LightSlateGray = 0xFF778899,
            LightSteelBlue = 0xFFB0C4DE,
            LightYellow = 0xFFFFFFE0,
            Lime = 0xFF00FF00,
            LimeGreen = 0xFF32CD32,
            Linen = 0xFFFAF0E6,
            //! 洋红、紫红、品红
            Magenta = 0xFFFF00FF,
            Maroon = 0xFF800000,
            MediumAquamarine = 0xFF66CDAA,
            MediumBlue = 0xFF0000CD,
            MediumOrchid = 0xFFBA55D3,
            MediumPurple = 0xFF9370DB,
            MediumSeaGreen = 0xFF3CB371,
            MediumSlateBlue = 0xFF7B68EE,
            MediumSpringGreen = 0xFF00FA9A,
            MediumTurquoise = 0xFF48D1CC,
            MediumVioletRed = 0xFFC71585,
            MidnightBlue = 0xFF191970,
            MintCream = 0xFFF5FFFA,
            MistyRose = 0xFFFFE4E1,
            Moccasin = 0xFFFFE4B5,
            NavajoWhite = 0xFFFFDEAD,
            Navy = 0xFF000080,
            OldLace = 0xFFFDF5E6,
            Olive = 0xFF808000,
            OliveDrab = 0xFF6B8E23,
            Orange = 0xFFFFA500,
            OrangeRed = 0xFFFF4500,
            Orchid = 0xFFDA70D6,
            PaleGoldenrod = 0xFFEEE8AA,
            PaleGreen = 0xFF98FB98,
            PaleTurquoise = 0xFFAFEEEE,
            PaleVioletRed = 0xFFDB7093,
            PapayaWhip = 0xFFFFEFD5,
            PeachPuff = 0xFFFFDAB9,
            Peru = 0xFFCD853F,
            Pink = 0xFFFFC0CB,
            Plum = 0xFFDDA0DD,
            PowderBlue = 0xFFB0E0E6,
            Purple = 0xFF800080,
            RosyBrown = 0xFFBC8F8F,
            RoyalBlue = 0xFF4169E1,
            SaddleBrown = 0xFF8B4513,
            Salmon = 0xFFFA8072,
            SandyBrown = 0xFFF4A460,
            SeaGreen = 0xFF2E8B57,
            SeaShell = 0xFFFFF5EE,
            Sienna = 0xFFA0522D,
            Silver = 0xFFC0C0C0,
            SkyBlue = 0xFF87CEEB,
            SlateBlue = 0xFF6A5ACD,
            SlateGray = 0xFF708090,
            //! 雪白
            Snow = 0xFFFFFAFA,
            //! 春绿色
            SpringGreen = 0xFF00FF7F,
            SteelBlue = 0xFF4682B4,
            Tan = 0xFFD2B48C,
            Teal = 0xFF008080,
            //! 蓟色
            Thistle = 0xFFD8BFD8,
            Tomato = 0xFFFF6347,
            Turquoise = 0xFF40E0D0,
            Violet = 0xFFEE82EE,
            Wheat = 0xFFF5DEB3,
            White = 0xFFFFFFFF,
            WhiteSmoke = 0xFFF5F5F5,
            Yellow = 0xFFFFFF00,
            YellowGreen = 0xFF9ACD32
        };
    };
}

