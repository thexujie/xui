#pragma once

#include "UspInc.h"

namespace win32
{
    namespace script
    {
        class UspFontCacheT
        {
        public:
            int32_t iHash;
            HFONT hFont;
            SCRIPT_CACHE * cache;

#ifdef _DEBUG
            font_t font;
#endif
        };

        struct UspFallBackFontT
        {
            graphics::text::font font;
            HFONT hFont;
        };

        typedef int32_t usp_cache_t;
        class CUspFactory : public UspBase
        {
        public:
            CUspFactory(HDC hdc);
            ~CUspFactory();

            ITextLayout * CreateLayout();

            usp_font_t GetFont(const font_t & font);
            void SetFont(const usp_font_t & font);
            usp_font_t GetFontFallBack(const font_t & font, int32_t iLanguage, const char_16 * text = nullptr, int32_t length = 0);
            bool RestFontFallBack();
            int32_t GetTabSize(const usp_font_t & font, int32_t iIndex);

        public:
            void AddFallBack(int32_t iCharset, const char_16 * szName);
        private:
            UspFontCacheT * GetFontCache(const font_t & font);
        private:
            vector<UspFontCacheT> m_fonts;
            HFONT m_hFontOld;

            vector<UspFallBackFontT> m_fallbacks;
        };

    }
}
