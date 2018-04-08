#include "stdafx.h"
#include "UspInc.h"

VENUS_BEG

UspBase::UspBase(HDC hdc)
{
	m_hdc = ::CreateCompatibleDC(hdc);
}

UspBase::~UspBase()
{
	::DeleteDC(m_hdc);
	m_hdc = NULL;
}

HDC UspBase::GetDC() const
{
	return m_hdc;
}

HRESULT UspBase::FreeCache(SCRIPT_CACHE * pCacle)
{
	if(!m_hModule)
		return E_FAIL;
	return m_pfnScriptFreeCache(pCacle);
}

HRESULT UspBase::IsComplex(const char_16 * text, int_x length, uint_32 flags)
{
	if(!m_hModule)
		return E_FAIL;
	return m_pfnScriptIsComplex(text, (int_32)length, flags);
}

HRESULT UspBase::Itemize(const char_16 * text, int_x length,
	SCRIPT_ITEM  * items, int_x nitem_max, int_x * pnitem,
	const SCRIPT_CONTROL * control, const SCRIPT_STATE * state)
{
	if(!m_hModule)
		return E_FAIL;
	int_32 iCount = 0;
	HRESULT hResult = m_pfnScriptItemize(text, (int_32)length, (int_32)nitem_max, control, state, items, &iCount);
	if(pnitem)
		*pnitem = iCount;
	return hResult;
}

HRESULT UspBase::Shape(SCRIPT_CACHE * cache, 
	const char_16 * text, int_x length, SCRIPT_ANALYSIS * analysis, uint_16 * text_clusters,
	uint_16 * glyphs, SCRIPT_VISATTR * glyph_attrs, int_x nglyph_max, int_x * pnglyph)
{
	if(!m_hModule)
		return E_FAIL;
	int_32 iGlyphs = 0;
	HRESULT hResult = m_pfnScriptShape(m_hdc, cache, text, (int_32)length, (int_32)nglyph_max, analysis, glyphs, text_clusters, glyph_attrs, &iGlyphs);
	if(pnglyph)
		*pnglyph = iGlyphs;
	return hResult;
}

HRESULT UspBase::Place(SCRIPT_CACHE * cache, SCRIPT_ANALYSIS * analysis,
	const uint_16 * glyphs, const SCRIPT_VISATTR * glyph_attrs, int_32 * advances, GOFFSET * offsets, int_x nglyph,
	ABC * abc)
{
	if(!m_hModule)
		return E_FAIL;
	return m_pfnScriptPlace(m_hdc, cache, glyphs, (int_32)nglyph, glyph_attrs, analysis, advances, offsets, abc);
}

HRESULT UspBase::Break(const char_16 * text, int_x length, const SCRIPT_ANALYSIS * analyses, SCRIPT_LOGATTR * text_attrs)
{
	if(!m_hModule)
		return E_FAIL;
	return m_pfnScriptBreak(text, (int_32)length, analyses, text_attrs);
}

HRESULT UspBase::GetFontProperties(SCRIPT_CACHE * cache, SCRIPT_FONTPROPERTIES * pprop)
{
	if(!m_hModule)
		return E_FAIL;
	return m_pfnScriptGetFontProperties(m_hdc, cache, pprop);
}

HRESULT UspBase::TextOutScp(const HDC hdc, SCRIPT_CACHE * cache,
	int_x x, int_x y, uint_32 options,
	const RECT * rect,
	const SCRIPT_ANALYSIS * analysis,
	const uint_16 * glyphs, int_x nglyph,
	const int_32 * advances,
	const int_32 * justfies,
	const GOFFSET * offsets)
{
	if(!m_hModule)
		return E_FAIL;
	return m_pfnScriptTextOut(hdc, cache, (int_32)x, (int_32)y, options, rect, analysis,
		/*szReserved*/NULL, /*iReserved*/0, 
		glyphs, (int_32)nglyph, advances, justfies, offsets);
}

bool UspBase::LoadUsp10()
{
	if(Load(L"usp10.dll"))
	{
		GetProc(m_pfnScriptFreeCache, "ScriptFreeCache");
		GetProc(m_pfnScriptIsComplex, "ScriptIsComplex");
		GetProc(m_pfnScriptItemize, "ScriptItemize");
		GetProc(m_pfnScriptShape, "ScriptShape");
		GetProc(m_pfnScriptPlace, "ScriptPlace");
		GetProc(m_pfnScriptBreak, "ScriptBreak");
		GetProc(m_pfnScriptGetFontProperties, "ScriptGetFontProperties");
		GetProc(m_pfnScriptTextOut, "ScriptTextOut");

		if(!m_pfnScriptIsComplex || !m_pfnScriptItemize || !m_pfnScriptShape ||
			!m_pfnScriptPlace || !m_pfnScriptBreak || !m_pfnScriptGetFontProperties || !m_pfnScriptTextOut)
		{
			Free();
			m_pfnScriptFreeCache = nullptr;
			m_pfnScriptIsComplex = nullptr;
			m_pfnScriptItemize = nullptr;
			m_pfnScriptShape = nullptr;
			m_pfnScriptPlace = nullptr;
			m_pfnScriptBreak = nullptr;
			m_pfnScriptGetFontProperties = nullptr;
			m_pfnScriptTextOut = nullptr;
			return false;
		}
		else
			return true;
	}
	else
		return false;
}

bool UspHasMissingGlyphs(const uint_16 * pGlyphs, int_x iLength, const SCRIPT_FONTPROPERTIES & sfp)
{
	uint_16 wDefault = sfp.wgDefault;
	uint_16 wInvalid = sfp.wgInvalid;
	uint_16 wBlank = sfp.wgBlank;
	uint_16 wGlyph = 0;
	for(int_x cnt = 0; cnt < iLength; ++cnt)
	{
		wGlyph = pGlyphs[cnt];
		if(wGlyph == wDefault ||
			(wGlyph == wInvalid && wInvalid != wBlank))
			return true;
	}
	return false;
}

VENUS_END
