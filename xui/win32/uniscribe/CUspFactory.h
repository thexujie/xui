#pragma once

#include "UspInc.h"

VENUS_BEG

class WIN32_API UspFontCacheT
{
public:
	int_x iHash;
	HFONT hFont;
	SCRIPT_CACHE * cache;

#ifdef _DEBUG
	font_t font;
#endif
};

struct UspFallBackFontT
{
	font_base_t font;
	HFONT hFont;
};

typedef int_x usp_cache_t;
class WIN32_API CUspFactory : public UspBase
{
public:
	CUspFactory(HDC hdc);
	~CUspFactory();

	ITextLayout * CreateLayout();

	usp_font_t GetFont(const font_t & font);
	void SetFont(const usp_font_t & font);
	usp_font_t GetFontFallBack(const font_t & font, int_x iLanguage, const char_16 * text = nullptr, int_x length = 0);
	bool RestFontFallBack();
	int_x GetTabSize(const usp_font_t & font, int_x iIndex);

public:
	void AddFallBack(int_x iCharset, const char_16 * szName);
private:
	UspFontCacheT * GetFontCache(const font_t & font);
private:
	vector<UspFontCacheT> m_fonts;
	HFONT m_hFontOld;

	vector<UspFallBackFontT> m_fallbacks;
};

VENUS_END
