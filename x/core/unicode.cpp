#include "stdafx.h"
#include "unicode.h"

namespace core
{
    const unicodeplane_t UNICODE_PLANE_INVALID = { unicodeplane_invalid, 0, 0, "invalid plane" };
    const unicodeplane_t UNICODE_PLANES[unicodeplane_count] =
    {
        { 0, 0x0000, 0x001F, "C0 Controls" }, // C0控制符	C0 Controls
        { 1, 0x0020, 0x007F, "Basic Latin" }, // 基本拉丁文	Basic Latin
        { 2, 0x0080, 0x009F, "C1 Controls" }, // C1控制符	C1 Controls
        { 3, 0x00A0, 0x00FF, "Latin 1 Supplement" }, // 拉丁文补充-1	Latin 1 Supplement
        { 4, 0x0100, 0x017F, "Latin Extended-A" }, // 拉丁文扩展-A	Latin Extended-A
        { 5, 0x0180, 0x024F, "Latin Extended-B" }, // 拉丁文扩展-B	Latin Extended-B
        { 6, 0x0250, 0x02AF, "IPA Extensions" }, // 国际音标扩展	IPA Extensions
        { 7, 0x02B0, 0x02FF, "Spacing Modifiers" }, // 占位修饰符号	Spacing Modifiers
        { 8, 0x0300, 0x036F, "Combining Diacritics Marks" }, // 结合附加符号	Combining Diacritics Marks
        { 9, 0x0370, 0x03FF, "Greek and Coptic" }, // 希腊字母及科普特字母	Greek and Coptic
        { 10, 0x0400, 0x04FF, "Cyrillic" }, // 西里尔字母	Cyrillic
        { 11, 0x0500, 0x052F, "Cyrillic Supplement" }, // 西里尔字母补充	Cyrillic Supplement
        { 12, 0x0530, 0x058F, "Armenian" }, // 亚美尼亚字母	Armenian
        { 13, 0x0590, 0x05FF, "Hebrew" }, // 希伯来文	Hebrew
        { 14, 0x0600, 0x06FF, "Arabic" }, // 阿拉伯文	Arabic
        { 15, 0x0700, 0x074F, "Syriac" }, // 叙利亚文	Syriac
        { 16, 0x0750, 0x077F, "Arabic Supplement" }, // 阿拉伯文补充	Arabic Supplement
        { 17, 0x0780, 0x07BF, "Thaana" }, // 它拿字母	Thaana
        { 18, 0x07C0, 0x07FF, "N'Ko" }, // 西非书面语言	N'Ko
        { 19, 0x0800, 0x083F, "Samaritan" }, // 撒玛利亚字母	Samaritan
        { 20, 0x0840, 0x085F, "Mandaic" }, // 曼达语	Mandaic
        { 21, 0x0860, 0x08FF, "" }, // 待定	
        { 22, 0x0900, 0x097F, "Devanagari" }, // 天城文	Devanagari
        { 23, 0x0980, 0x09FF, "Bengali" }, // 孟加拉文	Bengali
        { 24, 0x0A00, 0x0A7F, "Gurmukhi" }, // 果鲁穆奇字母	Gurmukhi
        { 25, 0x0A80, 0x0AFF, "Gujarati" }, // 古吉拉特文	Gujarati
        { 26, 0x0B00, 0x0B7F, "Oriya" }, // 奥里亚文	Oriya
        { 27, 0x0B80, 0x0BFF, "Tamil" }, // 泰米尔文	Tamil
        { 28, 0x0C00, 0x0C7F, "Telugu" }, // 泰卢固文	Telugu
        { 29, 0x0C80, 0x0CFF, "Kannada" }, // 卡纳达文	Kannada
        { 30, 0x0D00, 0x0D7F, "Malayalam" }, // 马拉雅拉姆文	Malayalam
        { 31, 0x0D80, 0x0DFF, "Sinhala" }, // 僧伽罗文	Sinhala
        { 32, 0x0E00, 0x0E7F, "Thai" }, // 泰文	Thai
        { 33, 0x0E80, 0x0EFF, "Lao" }, // 老挝文	Lao
        { 34, 0x0F00, 0x0FFF, "Tibetan" }, // 藏文	Tibetan
        { 35, 0x1000, 0x109F, "Myanmar" }, // 缅甸文	Myanmar
        { 36, 0x10A0, 0x10FF, "Georgian" }, // 格鲁吉亚字母	Georgian
        { 37, 0x1100, 0x11FF, "Hangul Jamo" }, // 谚文字母	Hangul Jamo
        { 38, 0x1200, 0x137F, "Ethiopic" }, // 埃塞俄比亚语	Ethiopic
        { 39, 0x1380, 0x139F, "Ethiopic Supplement" }, // 埃塞俄比亚语补充	Ethiopic Supplement
        { 40, 0x13A0, 0x13FF, "Cherokee" }, // 切罗基字母	Cherokee
        { 41, 0x1400, 0x167F, "Unified Canadian Aboriginal Syllabics" }, // 统一加拿大土著语音节	Unified Canadian Aboriginal Syllabics
        { 42, 0x1680, 0x169F, "Ogham" }, // 欧甘字母	Ogham
        { 43, 0x16A0, 0x16FF, "Runic" }, // 卢恩字母	Runic
        { 44, 0x1700, 0x171F, "Tagalog" }, // 他加禄字母	Tagalog
        { 45, 0x1720, 0x173F, "Hanunóo" }, // 哈努诺文	Hanunóo
        { 46, 0x1740, 0x175F, "Buhid" }, // 布迪文	Buhid
        { 47, 0x1760, 0x177F, "Tagbanwa" }, // 塔格巴努亚文	Tagbanwa
        { 48, 0x1780, 0x17FF, "Khmer" }, // 高棉文	Khmer
        { 49, 0x1800, 0x18AF, "Mongolian" }, // 蒙古文	Mongolian
        { 50, 0x18B0, 0x18FF, "Unified Canadian Aboriginal Syllabics Extended" }, // 加拿大原住民音节文字扩展	Unified Canadian Aboriginal Syllabics Extended
        { 51, 0x1900, 0x194F, "Limbu" }, // 林布文	Limbu
        { 52, 0x1950, 0x197F, "Tai Le" }, // 德宏傣文	Tai Le
        { 53, 0x1980, 0x19DF, "New Tai Lue" }, // 新傣仂文	New Tai Lue
        { 54, 0x19E0, 0x19FF, "Khmer Symbols" }, // 高棉文符号	Khmer Symbols
        { 55, 0x1A00, 0x1A1F, "Buginese" }, // 布吉文	Buginese
        { 56, 0x1A20, 0x1AAF, "Tai Tham" }, // 老傣文	Tai Tham
        { 57, 0x1AB0, 0x1AFF, "" }, // 待定	
        { 58, 0x1B00, 0x1B7F, "Balinese" }, // 巴厘字母	Balinese
        { 59, 0x1B80, 0x1BBF, "Sundanese" }, // 巽他字母	Sundanese
        { 60, 0x1BC0, 0x1BFF, "Batak" }, // 巴塔克文	Batak
        { 61, 0x1C00, 0x1C4F, "Lepcha" }, // 雷布查字母	Lepcha
        { 62, 0x1C50, 0x1C7F, "Ol Chiki" }, // 桑塔利文	Ol Chiki
        { 63, 0x1C80, 0x1CBF, "" }, // 待定	
        { 64, 0x1CC0, 0x1CCF, "Sudanese Supplement" }, // 巽他字母补充	Sudanese Supplement
        { 65, 0x1CD0, 0x1CFF, "Vedic Extensions" }, // 吠陀梵文	Vedic Extensions
        { 66, 0x1D00, 0x1D7F, "Phonetic Extensions" }, // 语音学扩展	Phonetic Extensions
        { 67, 0x1D80, 0x1DBF, "Phonetic Extensions Supplement" }, // 语音学扩展补充	Phonetic Extensions Supplement
        { 68, 0x1DC0, 0x1DFF, "Combining Diacritics Marks Supplement" }, // 结合附加符号补充	Combining Diacritics Marks Supplement
        { 69, 0x1E00, 0x1EFF, "Latin Extended Additional" }, // 拉丁文扩展附加	Latin Extended Additional
        { 70, 0x1F00, 0x1FFF, "Greek Extended" }, // 希腊语扩展	Greek Extended
        { 71, 0x2000, 0x206F, "General Punctuation" }, // 常用标点	General Punctuation
        { 72, 0x2070, 0x209F, "Superscripts and Subscripts" }, // 上标及下标	Superscripts and Subscripts
        { 73, 0x20A0, 0x20CF, "Currency Symbols" }, // 货币符号	Currency Symbols
        { 74, 0x20D0, 0x20FF, "Combining Diacritics Marks for Symbols" }, // 组合用记号	Combining Diacritics Marks for Symbols
        { 75, 0x2100, 0x214F, "Letterlike Symbols" }, // 字母式符号	Letterlike Symbols
        { 76, 0x2150, 0x218F, "Number Form" }, // 数字形式	Number Form
        { 77, 0x2190, 0x21FF, "Arrows" }, // 箭头	Arrows
        { 78, 0x2200, 0x22FF, "Mathematical Operator" }, // 数学运算符	Mathematical Operator
        { 79, 0x2300, 0x23FF, "Miscellaneous Technical" }, // 杂项工业符号	Miscellaneous Technical
        { 80, 0x2400, 0x243F, "Control Pictures" }, // 控制图片	View Pictures
        { 81, 0x2440, 0x245F, "Optical Character Recognition" }, // 光学识别符	Optical Character Recognition
        { 82, 0x2460, 0x24FF, "Enclosed Alphanumerics" }, // 封闭式字母数字	Enclosed Alphanumerics
        { 83, 0x2500, 0x257F, "Box Drawing" }, // 制表符	Box Drawing
        { 84, 0x2580, 0x259F, "Block Element" }, // 方块元素	Block Element
        { 85, 0x25A0, 0x25FF, "Geometric Shapes" }, // 几何图形	Geometric Shapes
        { 86, 0x2600, 0x26FF, "Miscellaneous Symbols" }, // 杂项符号	Miscellaneous Symbols
        { 87, 0x2700, 0x27BF, "Dingbats" }, // 印刷符号	Dingbats
        { 88, 0x27C0, 0x27EF, "Miscellaneous Mathematical Symbols-A" }, // 杂项数学符号-A	Miscellaneous Mathematical Symbols-A
        { 89, 0x27F0, 0x27FF, "Supplemental Arrows-A" }, // 追加箭头-A	Supplemental Arrows-A
        { 90, 0x2800, 0x28FF, "Braille Patterns" }, // 盲文点字模型	Braille Patterns
        { 91, 0x2900, 0x297F, "Supplemental Arrows-B" }, // 追加箭头-B	Supplemental Arrows-B
        { 92, 0x2980, 0x29FF, "Miscellaneous Mathematical Symbols-B" }, // 杂项数学符号-B	Miscellaneous Mathematical Symbols-B
        { 93, 0x2A00, 0x2AFF, "Supplemental Mathematical Operator" }, // 追加数学运算符	Supplemental Mathematical Operator
        { 94, 0x2B00, 0x2BFF, "Miscellaneous Symbols and Arrows" }, // 杂项符号和箭头	Miscellaneous Symbols and Arrows
        { 95, 0x2C00, 0x2C5F, "Glagolitic" }, // 格拉哥里字母	Glagolitic
        { 96, 0x2C60, 0x2C7F, "Latin Extended-C" }, // 拉丁文扩展-C	Latin Extended-C
        { 97, 0x2C80, 0x2CFF, "Coptic" }, // 科普特字母	Coptic
        { 98, 0x2D00, 0x2D2F, "Georgian Supplement" }, // 格鲁吉亚字母补充	Georgian Supplement
        { 99, 0x2D30, 0x2D7F, "Tifinagh" }, // 提非纳文	Tifinagh
        { 100, 0x2D80, 0x2DDF, "Ethiopic Extended" }, // 埃塞俄比亚语扩展	Ethiopic Extended
        { 101, 0x2E00, 0x2E7F, "Supplemental Punctuation" }, // 追加标点	Supplemental Punctuation
        { 102, 0x2E80, 0x2EFF, "CJK Radicals Supplement" }, // 中日韩部首补充	CJK Radicals Supplement
        { 103, 0x2F00, 0x2FDF, "Kangxi Radicals" }, // 康熙部首	Kangxi Radicals
        { 104, 0x2FF0, 0x2FFF, "Ideographic Description Characters" }, // 表意文字描述符	Ideographic Description Characters
        { 105, 0x3000, 0x303F, "CJK Symbols and Punctuation" }, // 中日韩符号和标点	CJK Symbols and Punctuation
        { 106, 0x3040, 0x309F, "Hiragana" }, // 日文平假名	Hiragana
        { 107, 0x30A0, 0x30FF, "Katakana" }, // 日文片假名	Katakana
        { 108, 0x3100, 0x312F, "Bopomofo" }, // 注音字母	Bopomofo
        { 109, 0x3130, 0x318F, "Hangul Compatibility Jamo" }, // 谚文兼容字母	Hangul Compatibility Jamo
        { 110, 0x3190, 0x319F, "Kanbun" }, // 象形字注释标志	Kanbun
        { 111, 0x31A0, 0x31BF, "Bopomofo Extended" }, // 注音字母扩展	Bopomofo Extended
        { 112, 0x31C0, 0x31EF, "CJK Strokes" }, // 中日韩笔画	CJK Strokes
        { 113, 0x31F0, 0x31FF, "Katakana Phonetic Extensions" }, // 日文片假名语音扩展	Katakana Phonetic Extensions
        { 114, 0x3200, 0x32FF, "Enclosed CJK Letters and Months" }, // 带圈中日韩字母和月份	Enclosed CJK Letters and Months
        { 115, 0x3300, 0x33FF, "CJK Compatibility" }, // 中日韩兼容	CJK Compatibility
        { 116, 0x3400, 0x4DBF, "CJK Unified Ideographs Extension A" }, // 中日韩统一表意文字扩展A	 CJK Unified Ideographs Extension A
        { 117, 0x4DC0, 0x4DFF, "Yijing Hexagrams Symbols" }, // 易经六十四卦符号	Yijing Hexagrams Symbols
        { 118, 0x4E00, 0x9FFF, "CJK Unified Ideographs" }, // 中日韩统一表意文字	CJK Unified Ideographs
        { 119, 0xA000, 0xA48F, "Yi Syllables" }, // 彝文音节	Yi Syllables
        { 120, 0xA490, 0xA4CF, "Yi Radicals" }, // 彝文字根	Yi Radicals
        { 121, 0xA4D0, 0xA4FF, "Lisu" }, // 老傈僳文	Lisu
        { 122, 0xA500, 0xA63F, "Vai" }, // 瓦伊语	Vai
        { 123, 0xA640, 0xA69F, "Cyrillic Extended-B" }, // 西里尔字母扩展-B	Cyrillic Extended-B
        { 124, 0xA6A0, 0xA6FF, "Bamum" }, // 巴姆穆语	Bamum
        { 125, 0xA700, 0xA71F, "Modifier Tone Letters" }, // 声调修饰字母	Modifier Tone Letters
        { 126, 0xA720, 0xA7FF, "Latin Extended-D" }, // 拉丁文扩展-D	Latin Extended-D
        { 127, 0xA800, 0xA82F, "Syloti Nagri" }, // 锡尔赫特文	Syloti Nagri
        { 128, 0xA830, 0xA83F, "Ind. No." }, // 	Ind. No.
        { 129, 0xA840, 0xA87F, "Phags-pa" }, // 八思巴字	Phags-pa
        { 130, 0xA880, 0xA8DF, "Saurashtra" }, // 索拉什特拉	Saurashtra
        { 131, 0xA8E0, 0xA8FF, "Deva. Ext." }, // 	Deva. Ext.
        { 132, 0xA900, 0xA92F, "Kayah Li" }, // 克耶字母	Kayah Li
        { 133, 0xA930, 0xA95F, "Rejang" }, // 勒姜语	Rejang
        { 134, 0xA980, 0xA9DF, "Javanese" }, // 爪哇语	Javanese
        { 135, 0xA9E0, 0xA9FF, "" }, // 待定	
        { 136, 0xAA00, 0xAA5F, "Cham" }, // 占语字母	Cham
        { 137, 0xAA60, 0xAA7F, "Myanmar ExtA" }, // 缅甸语扩展	Myanmar ExtA
        { 138, 0xAA80, 0xAADF, "Tai Viet" }, // 越南傣文	Tai Viet
        { 139, 0xAAE0, 0xAAFF, "Meetei Ext" }, // 曼尼普尔文扩展	Meetei Ext
        { 140, 0xAB00, 0xAB2F, "Ethiopic Ext-A" }, // 埃塞俄比亚文	Ethiopic Ext-A
        { 141, 0xAB30, 0xABBF, "" }, // 待定	
        { 142, 0xABC0, 0xABFF, "Meetei Mayek" }, // 	Meetei Mayek
        { 143, 0xAC00, 0xD7AF, "Hangul Syllables" }, // 谚文音节	Hangul Syllables
        { 144, 0xD7B0, 0xD7FF, "Hangul Jamo Extended-B" }, // 谚文字母扩展-B	Hangul Jamo Extended-B
        { 145, 0xD800, 0xDBFF, "High-half zone of UTF-16" }, // 	High-half zone of UTF-16
        { 146, 0xDC00, 0xDFFF, "Low-half zone of UTF-16" }, // 	Low-half zone of UTF-16
        { 147, 0xE000, 0xF8FF, "Private Use Zone" }, // 自行使用区域	Private Use Zone
        { 148, 0xF900, 0xFAFF, "CJK Compatibility Ideographs" }, // 中日韩兼容表意文字	CJK Compatibility Ideographs
        { 149, 0xFB00, 0xFB4F, "Alphabetic Presentation Forms" }, // 字母表达形式（拉丁字母连字、亚美尼亚字母连字、希伯来文表现形式）	Alphabetic Presentation Forms
        { 150, 0xFB50, 0xFDFF, "Arabic Presentation Forms A" }, // 阿拉伯文表达形式A	Arabic Presentation Forms A
        { 151, 0xFE00, 0xFE0F, "Variation Selector" }, // 异体字选择符	Variation Selector
        { 152, 0xFE10, 0xFE1F, "Vertical Forms" }, // 竖排形式	Vertical Forms
        { 153, 0xFE20, 0xFE2F, "Combining Half Marks" }, // 组合用半符号	Combining Half Marks
        { 154, 0xFE30, 0xFE4F, "CJK Compatibility Forms" }, // 中日韩兼容形式	CJK Compatibility Forms
        { 155, 0xFE50, 0xFE6F, "Small Form Variants" }, // 小写变体形式	Small Form Variants
        { 156, 0xFE70, 0xFEFF, "Arabic Presentation Forms B" }, // 阿拉伯文表达形式B	Arabic Presentation Forms B
        { 157, 0xFF00, 0xFFEF, "Halfwidth and Fullwidth Forms" }, // 半角及全角	Halfwidth and Fullwidth Forms
        { 158, 0xFFF0, 0xFFFF, "Specials" }, // 特殊	Specials
        { 159, 0x10000, 0x100FF, "Linear B" }, // 线形文字B	Linear B
        { 160, 0x10100, 0x101CF, "Ancient numeric systems" }, // 古代记数系统	Ancient numeric systems
        { 161, 0x101D0, 0x107FF, "Alphabetic and syllabic LTR scripts and sets of symbols" }, // 从左向右书写的音素和音节文字和符号集	Alphabetic and syllabic LTR scripts and sets of symbols
        { 162, 0x10800, 0x10FFF, "Alphabetic and syllabic RTL scripts" }, // 从右向左书写的音素和音节文字	Alphabetic and syllabic RTL scripts
        { 163, 0x11000, 0x11FFF, "Brahmic scripts" }, // 婆罗米文字	Brahmic scripts
        { 164, 0x12000, 0x12FFF, "Cuneiform and other ancient scripts" }, // 楔形文字和其他古代文字	Cuneiform and other ancient scripts
        { 165, 0x13000, 0x15BFF, "Egyptian and Maya hieroglyphs" }, // 埃及和玛雅圣书体	Egyptian and Maya hieroglyphs
        { 166, 0x15C00, 0x15FFF, "Aztec pictograms" }, // 阿兹台克象形文字	Aztec pictograms
        { 167, 0x16000, 0x16FFF, "Recently-devised scripts" }, // 新创文字	Recently-devised scripts
        { 168, 0x17000, 0x1B5FF, "Large Asian scripts" }, // 亚洲大文字	Large Asian scripts
        { 169, 0x1B600, 0x1BFFF, "unassigned" }, // 未分配	unassigned
        { 170, 0x1C000, 0x1CDFF, "Micmac hieroglyphs" }, // 米格马赫圣书体	Micmac hieroglyphs
        { 171, 0x1CE00, 0x1CFFF, "Proto-Elamite" }, // 原始埃兰文字	Proto-Elamite
        { 172, 0x1D000, 0x1DFFF, "Notational systems" }, // 记号系统	Notational systems
        { 173, 0x1E000, 0x1E7FF, "unassigned" }, // 未分配	unassigned
        { 174, 0x1E800, 0x1EFFF, "RTL scripts" }, // 从右向左书写的文字	RTL scripts
        { 175, 0x1F000, 0x1F0FF, "Game symbols" }, // 游戏符号	Game symbols
        { 176, 0x1F100, 0x1F2FF, "Alphanumeric and ideographic sets" }, // 字母数字和表意字符集	Alphanumeric and ideographic sets
        { 177, 0x1F300, 0x1F7FF, "Pictographic sets" }, // 象形字符集	Pictographic sets
        { 178, 0x1F800, 0x1FFFD, "unassigned" }, // 未分配	unassigned

        // 中日韩统一表意文字扩展B CJK Unified Ideographs Extension B
        { 179, 0x20000, 0x2A6D6, "CJK Unified Ideographs Extension B" },
        { 180, 0x2F800, 0x2FA1D, "CJK Compatibility Supplement" },

    };

    const unicodeplane_t & unicodeplane_get(int32_t plane)
    {
        return UNICODE_PLANES[plane];
    }

    const unicodeplane_t & unicodeplane_find(char32_t ch)
    {
        const int32_t COUNT = sizeof(UNICODE_PLANES) / sizeof(unicodeplane_t);
        for (int32_t cnt = 0; cnt < COUNT; ++cnt)
        {
            const unicodeplane_t & plane = UNICODE_PLANES[cnt];
            if (plane.beg <= ch && ch <= plane.end)
                return plane;
        }
        return UNICODE_PLANE_INVALID;
    }

    const unicodeplane_t & unicodeplane_find(const char16_t * text, int32_t length)
    {
        char32_t ch;
        if (utf16_to_unicode(text, length, ch) > 0)
            return unicodeplane_find(ch);
        else
            return UNICODE_PLANE_INVALID;
    }

    int32_t unicode_to_utf16(char32_t ch, char16_t * text, int32_t size)
    {
        if (size < 0)
            size = textlen(text, 2);

        if (ch > 0xffff)
        {
            ch -= 0x10000;
            if (size >= 2)
            {
                const unicodeplane_t & plane_h = UNICODE_PLANES[unicodeplane_utf16_high];
                const unicodeplane_t & plane_l = UNICODE_PLANES[unicodeplane_utf16_low];
                text[0] = (ch >> 10) + plane_h.beg;
                text[1] = (ch & 0x03ff) + plane_l.beg;
                return 2;
            }
        }
        else
        {
            if (size >= 1)
            {
                *text = (char16_t)ch;
                return 1;
            }
        }
        return 0;
    }

    int32_t utf16_to_unicode(const char16_t * text, int32_t length, char32_t & ch)
    {
        if (length < 0)
            length = textlen(text, 2);

        const unicodeplane_t & plane_h = UNICODE_PLANES[unicodeplane_utf16_high];
        const unicodeplane_t & plane_l = UNICODE_PLANES[unicodeplane_utf16_low];
        if (length == 0)
            ch = 0;
        else if (length == 1)
        {
            char16_t ch0 = text[0];
            if (plane_h.beg <= ch0 && ch0 <= plane_h.end)
                ch = 0;
            else if (plane_l.beg <= ch0 && ch0 <= plane_l.end)
                ch = 0;
            else
                ch = ch0;
        }
        else if (length >= 2)
        {
            char16_t ch0 = text[0];
            char16_t ch1 = text[1];
            if (plane_h.beg <= ch0 && ch0 <= plane_h.end)
            {
                if (plane_l.beg <= ch1 && ch1 <= plane_l.end)
                {
                    ch0 -= plane_h.beg;
                    ch1 -= plane_l.beg;
                    ch = (ch1 | (ch0 << 10)) + 0x10000;
                }
                else
                    ch = 0;
            }
            else
                ch = ch0;
        }
        else
            ch = 0;

        if (ch > 0xffff)
            return 2;
        else if (ch)
            return 1;
        else
            return 0;
    }
}
