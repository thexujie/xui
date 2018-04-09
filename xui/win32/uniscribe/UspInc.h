#pragma once

#include "graphics/text/text.h"
#include "win32/dll.h"
#include <usp10.h>

// usp10 动态加载.
//

class UspBase : public CModule
{
public:
	UspBase(HDC hdc);
	~UspBase();

	HDC GetDC() const;
	HRESULT FreeCache(SCRIPT_CACHE * cache);
	HRESULT IsComplex(const char_16 * text, int_x length, uint_32 flags);
	HRESULT Itemize(const char_16 * text, int_x length,
		SCRIPT_ITEM  * items, int_x nitem_max, int_x * pnitem,
		const SCRIPT_CONTROL * control, const SCRIPT_STATE * state);

	HRESULT Shape(SCRIPT_CACHE * cache, 
		const char_16 * text, int_x length, SCRIPT_ANALYSIS * analysis, uint_16 * text_clusters,
		uint_16 * glyphs, SCRIPT_VISATTR * glyph_attrs, int_x nglyph_max, int_x * pnglyph);

	HRESULT Place(SCRIPT_CACHE * cache, SCRIPT_ANALYSIS * analysis,
		const uint_16 * glyphs, const SCRIPT_VISATTR * glyph_attrs, int_32 * advances, GOFFSET * offsets, int_x nglyph,
		ABC * abc);

	HRESULT Break(const char_16 * text, int_x length, const SCRIPT_ANALYSIS * analyses, SCRIPT_LOGATTR * text_attrs);
	HRESULT GetFontProperties(SCRIPT_CACHE * cache, SCRIPT_FONTPROPERTIES * pprop);
	HRESULT TextOutScp(const HDC hdc, SCRIPT_CACHE * cache,
		int_x x, int_x y, uint_32 options,
		const RECT * rect,
		const SCRIPT_ANALYSIS * analysis,
		const uint_16 * glyphs, int_x nglyph,
		const int_32 * advances,
		const int_32 * justfies,
		const GOFFSET * offsets);
	bool LoadUsp10();

protected:
	HDC m_hdc;

    win32::dll _dll;

	decltype(ScriptFreeCache) * m_pfnScriptFreeCache;
	proc_ptr<decltype(ScriptIsComplex)> m_pfnScriptIsComplex;
	proc_ptr<decltype(ScriptItemize)> m_pfnScriptItemize;
	proc_ptr<decltype(ScriptShape)> m_pfnScriptShape;
	proc_ptr<decltype(ScriptPlace)> m_pfnScriptPlace;
	proc_ptr<decltype(ScriptBreak)> m_pfnScriptBreak;
	proc_ptr<decltype(ScriptGetFontProperties)> m_pfnScriptGetFontProperties;
	proc_ptr<decltype(ScriptTextOut)> m_pfnScriptTextOut;
};

struct usp_font_t
{
	SCRIPT_CACHE * cache;
	HFONT hfont;
	int_x font_size;
};

class  usp_run_t
{
public:
	trange_t trange;
	grange_t grange;
	crange_t crange;
	SCRIPT_ANALYSIS analysis;
	usp_font_t font;
};

class  scp_cluster_t
{
public:
	int_x run;
	trange_t trange;
	grange_t grange;
	int_x width;

	bool whitespace : 1;
	//! 该字符后是否是建议的换行位置.
	bool softbreak : 1;
	//! 是否是段落标记
	bool paragraphtag : 1;
	//! 是否是连字符
	bool softhyphen : 1;
	//! 是否是从右到左的阅读顺序
	bool right2left : 1;
	//! 是否是行标记
	bool linetag : 1;
};

struct usp_cluster_t : public tl_cluster_t
{
	int_x run;
	grange_t grange;
};

bool UspHasMissingGlyphs(const uint_16 * pGlyphs, int_x iLength, const SCRIPT_FONTPROPERTIES & sfp);

class  ITextLayoutUsp : public ITextLayout
{
public:
	virtual winerr_t Draw(HDC hdc, int_x iX, int_x iY, uint_32 color, const rectix & rect) const = 0;
};

VENUS_END
