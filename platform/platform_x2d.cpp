#include "stdafx.h"
#include "platform.h"
#include "x2d/x2d.h"

namespace drawing
{
	extern void __fontmetrics(const drawing::font & font, drawing::fontmetrics & metrics);
}

namespace platform
{
	static drawing::font __default_font;
	static drawing::fontmetrics __default_fontmetrics;
	static std::once_flag __default_font_once_flag;

	drawing::font default_font()
	{
		NONCLIENTMETRICSW metrics = { sizeof(NONCLIENTMETRICSW) };
		SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, metrics.cbSize, &metrics, 0);
		__default_font.family = core::wstr_u8str(metrics.lfMessageFont.lfFaceName);
		if (metrics.lfMessageFont.lfHeight < 0)
			__default_font.size = (float32_t)-metrics.lfMessageFont.lfHeight;
		else
		{
			HDC hdc = GetDC(HWND_DESKTOP);
			int dpiy = GetDeviceCaps(hdc, LOGPIXELSY);
			__default_font.size = metrics.lfMessageFont.lfHeight * 96.0f / dpiy;
			ReleaseDC(HWND_DESKTOP, hdc);
		}
		return __default_font;
	}

	void font_metrics(const drawing::font & font, drawing::fontmetrics & metrics)
	{
		__fontmetrics(font, metrics);
	}

	LOGFONTW MappingFont(HDC hdc, const drawing::font & font)
	{
		LOGFONTW logfont = {};
		if (font.family[0])
			core::textcpy(logfont.lfFaceName, LF_FACESIZE, core::u8str_wstr(font.family).data(), -1);
		else
			core::textcpy(logfont.lfFaceName, LF_FACESIZE, core::u8str_wstr(default_font().family).data(), -1);

		logfont.lfWidth = 0;
		if (font.size > 0)
		{
			int iDpiY = GetDeviceCaps(hdc, LOGPIXELSY);
			logfont.lfHeight = -static_cast<int32_t>(font.size * iDpiY / 72);
		}
		else
			logfont.lfHeight = std::lroundf(font.size);

		logfont.lfWeight = static_cast<int32_t>(font.style.weight);

		logfont.lfItalic = static_cast<uint8_t>(font.style.slant);
		logfont.lfUnderline = static_cast<uint8_t>(0);
		//logfont.lfStrikeOut = static_cast<uint8_t>(font.flags & font::underline);
		//logFont.lfCharSet = static_cast<uint8_t>(font.charset);
		logfont.lfCharSet = DEFAULT_CHARSET;
		logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
		logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;

		logfont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
		return logfont;
	}

	LOGFONTW MappingFont(const drawing::font & font)
	{
		HDC hdcScreen = GetDC(NULL);
		LOGFONTW logfont = MappingFont(hdcScreen, font);
		ReleaseDC(NULL, hdcScreen);
		return logfont;
	}
}
