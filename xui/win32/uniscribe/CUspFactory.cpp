#include "stdafx.h"
#include "CUspFactory.h"
#include "CTextLayoutUsp.h"

namespace win32
{
    namespace script
    {
        const usp_font_t USP_FONT_EMPTY = { 0, 0, 0 };
        CUspFactory::CUspFactory(HDC hdc)
            : UspBase(hdc), m_hFontOld(NULL)
        {

        }

        CUspFactory::~CUspFactory()
        {
            if (m_hFontOld)
            {
                ::SelectObject(m_hdc, m_hFontOld);
                m_hFontOld = NULL;
            }

            for (int_x cnt = 0, len = m_fonts.size(); cnt < len; ++cnt)
            {
                UspFontCacheT & cache = m_fonts[cnt];
                FreeCache(cache.cache);
                SafeDelete(cache.cache);
                ::DeleteObject(cache.hFont);
            }
            m_fonts.clear();
        }

        ITextLayout * CUspFactory::CreateLayout()
        {
            return new CTextLayoutUsp(this);
        }

        usp_font_t CUspFactory::GetFont(const font_t & font)
        {
            UspFontCacheT * pFontCache = GetFontCache(font);
            if (pFontCache)
            {
                usp_font_t uspFont = { pFontCache->cache, pFontCache->hFont, font.size };
                return uspFont;
            }
            else
                return usp_font_t();
        }

        void CUspFactory::SetFont(const usp_font_t & font)
        {
            HFONT hOldFont = (HFONT)::SelectObject(m_hdc, font.hfont);
            if (!m_hFontOld)
                m_hFontOld = hOldFont;
        }

        struct usp_enum_t
        {
            HDC hdc;
            char_32 ch;
            font_base_t font;
        };

        static int CALLBACK CUspFactory_EnumFontFamExProc(
            const LOGFONT    *lpelfe,
            const TEXTMETRIC *lpntme,
            DWORD      FontType,
            LPARAM     lParam
        )
        {
            usp_enum_t & usp = *(usp_enum_t *)lParam;
            LOGFONTW logfont = {};
            logfont.lfHeight = -30;
            logfont.lfCharSet = DEFAULT_CHARSET;
            logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
            logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
            logfont.lfQuality = CLEARTYPE_QUALITY;
            logfont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
            textcpy(logfont.lfFaceName, LF_FACESIZE, lpelfe->lfFaceName, -1);

            HFONT hFont = CreateFontIndirectW(&logfont);
            HGDIOBJ hFontOld = ::SelectObject(usp.hdc, hFont);

            ABC abc = {};
            BOOL bExists = GetCharABCWidthsW(usp.hdc, usp.ch, usp.ch, &abc);
            SelectObject(usp.hdc, hFontOld);

            DeleteObject(hFont);

            if (bExists)
            {
                usp.font.charset = DEFAULT_CHARSET;
                usp.font.name = lpelfe->lfFaceName;
            }
            return !bExists;
        }

        font_base_t FindFallBackFont(HDC hdc, char_32 ch)
        {
            LOGFONT logfont = {};
            logfont.lfFaceName[0] = '\0';
            logfont.lfCharSet = DEFAULT_CHARSET;
            logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
            logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
            logfont.lfQuality = CLEARTYPE_QUALITY;
            logfont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
            HDC hdcFind = CreateCompatibleDC(hdc);
            usp_enum_t usp = { hdcFind, ch };
            EnumFontFamiliesExW(hdc, &logfont, CUspFactory_EnumFontFamExProc, (LPARAM)&usp, 0);
            DeleteDC(hdcFind);
            return usp.font;
        }

        usp_font_t CUspFactory::GetFontFallBack(const font_t & font, int_x iLanguage, const char_16 * text, int_x length)
        {
            char_32 ch;
            int_x len = utf16_to_unicode(text, length, ch);
            if (len <= 0)
                return USP_FONT_EMPTY;

            font_t fontfb = font;
            const  unicodeplane_t & plane = unicodeplane_find(text, len);
            switch (plane.plane)
            {
            case unicodeplane_arabic: // lang 26
                fontfb.name = L"Times New Roman";
                return GetFont(fontfb);

            case unicodeplane_sinhala: // lang 31
                fontfb.name = L"Tahoma";
                return GetFont(fontfb);

            case unicodeplane_hangul_syllables: // lang 19
                fontfb.name = L"Malgun Gothic";
                return GetFont(fontfb);

            case unicodeplane_cjk_unified_ideographs_ext_b: // lang 12
                fontfb.name = L"SimSun-ExtB";
                return GetFont(fontfb);

            default:
                return USP_FONT_EMPTY;
            }
            //font_base_t fontfb = FindFallBackFont(m_hdc, ch);

            //if(fontfb.name[0])
            //{
            //	font_t fontfb = font;
            //	fontfb.charset = fontfb.charset;
            //	fontfb.name = fontfb.name;
            //	fontfb.size = fontfb.size;
            //	return GetFont(fontfb);
            //}
            //else
            //	return USP_FONT_EMPTY;

            if (iLanguage == 12)
            {
                font_t fontfb = font;
                fontfb.name = L"SimSun-ExtB";
                fontfb.size = fontfb.size;
                return GetFont(fontfb);
            }
            else if (iLanguage == 31)
            {
                font_t fontfb = font;
                fontfb.name = L"Tahoma";
                fontfb.size = fontfb.size;
                return GetFont(fontfb);
            }
            else if (iLanguage == 19)
            {
                font_t fontfb = font;
                fontfb.name = L"Malgun Gothic";
                fontfb.size = fontfb.size;
                return GetFont(fontfb);
            }
            else if (iLanguage == 26)
            {
                font_t fontfb = font;
                fontfb.name = L"Times New Roman";
                fontfb.size = fontfb.size;
                return GetFont(fontfb);
            }
            return USP_FONT_EMPTY;
        }

        int_x CUspFactory::GetTabSize(const usp_font_t & font, int_x iIndex)
        {
            return font.font_size * 4 * (1 + iIndex);
        }

        UspFontCacheT * CUspFactory::GetFontCache(const font_t & font)
        {
            int_x iHash = font.Hash();
            for (int_x cnt = 0, len = m_fonts.size(); cnt < len; ++cnt)
            {
                UspFontCacheT & cache = m_fonts[cnt];
                if (cache.iHash == iHash)
                    return &cache;
            }

            HDC hdc = m_hdc;
            LOGFONT logFont = {};
            FontToLOGFONT(hdc, font, logFont);
            HFONT hFont = CreateFontIndirect(&logFont);
            if (!hFont)
                throw exp_bad_state();

#ifdef _DEBUG
            UspFontCacheT cache = { iHash, hFont, NULL, font };
#else
            UspFontCacheT cache = { iHash, hFont, NULL };
#endif
            cache.cache = new SCRIPT_CACHE;
            *cache.cache = nullptr;
            return &m_fonts.add(cache);
            //#ifdef _DEBUG
            //	cFont.font = font;
            //#endif
        }

        void CUspFactory::AddFallBack(int_x iCharset, const char_16 * szName)
        {
            UspFallBackFontT & fallback = m_fallbacks.add();
            fallback.font.charset = iCharset;
            fallback.font.name = szName;
            fallback.hFont = nullptr;
        }
    }
}

