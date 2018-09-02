#include "stdafx.h"
#include "windows.h"

namespace win32
{
    std::string winerr_str(int32_t err)
    {
        wchar_t buffer[512] = {};
        int nchars = ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, 512, NULL);
        std::wstring str(buffer, nchars - 2);
        return core::wstr_u8str(str);
    }

    int runLoop()
    {
        MSG msg = {};
        while (msg.message != WM_QUIT)
        {
            DWORD dwWait = MsgWaitForMultipleObjectsEx(0, NULL, INFINITE, QS_ALLEVENTS, MWMO_ALERTABLE);
            switch (dwWait)
            {
            default:
                break;
            }

            while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                    break;
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
        return (int)msg.wParam;
    }

    u8string GUID2String(const GUID & guid)
    {
        std::array<char, 40> output;
        snprintf(output.data(), output.size(), "{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}",
            guid.Data1, guid.Data2, guid.Data3,
            guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6],
            guid.Data4[7]);
        return u8string(output.data());
    }

    u8string GUID2String(std::array<uint8_t, 16> data)
    {
        const GUID & guid = *(const GUID *)data.data();
        std::array<char, 40> output;
        snprintf(output.data(), output.size(), "{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}",
            guid.Data1, guid.Data2, guid.Data3,
            guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6],
            guid.Data4[7]);
        return u8string(output.data());
    }


    std::tuple<std::shared_ptr<byte_t>, int32_t> load_res(uint32_t id, res_e type)
    {
        static const char RES_TYPES[res_count][16] = { "FILE" };

        int index = std::clamp((int)type, 0, res_count - 1);
        HRSRC hRes = FindResourceA(NULL, (const char *)(uintmax_t)id, RES_TYPES[index]);
        if (hRes)
        {
            HGLOBAL hData = LoadResource(NULL, hRes);
            if (hData)
            {
                byte_t * ptr = (byte_t *)LockResource(hData);
                uint32_t nbRes = SizeofResource(NULL, hRes);

                std::shared_ptr<byte_t> data(ptr, [hData](byte_t * ptr)
                {
                    UnlockResource(hData);
                    FreeResource(hData);
                });
                return { data, (int32_t)nbRes };
            }
        }
        return {};
    }

    winversion_t version()
    {
        static winversion_t version = {};
        if (version.major == 0)
        {
            typedef LONG(WINAPI * RtlGetVersionFuncT)(RTL_OSVERSIONINFOEXW *);
            typedef void (WINAPI * NtVersionNumbersFuncT)(DWORD *, DWORD *, DWORD *);

            RTL_OSVERSIONINFOEXW rtlOSVersion = { sizeof(RTL_OSVERSIONINFOEXW) };
            RtlGetVersionFuncT pfnRtlGetVersion = (RtlGetVersionFuncT)(::GetProcAddress(::GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion"));
            if(pfnRtlGetVersion)
            {
                pfnRtlGetVersion(&rtlOSVersion);
            }
            
            if (rtlOSVersion.dwMajorVersion > 5)
            {
                version.major = rtlOSVersion.dwMajorVersion;
                version.minor = rtlOSVersion.dwMinorVersion;
                version.pack = rtlOSVersion.wServicePackMajor;
                version.build = rtlOSVersion.dwBuildNumber;
            }
            else
            {
                NtVersionNumbersFuncT pfnRtlGetNtVersionNumbers = (NtVersionNumbersFuncT)(::GetProcAddress(::GetModuleHandleW(L"ntdll.dll"), "RtlGetNtVersionNumbers"));
                if (pfnRtlGetNtVersionNumbers)
                {
                    DWORD dwMajor = 0;
                    DWORD dwMinor = 0;
                    DWORD dwBuild = 0;
                    pfnRtlGetNtVersionNumbers(&dwMajor, &dwMinor, &dwBuild);
                    dwBuild &= 0xffff;
                    version.major = dwMajor;
                    version.minor = dwMinor;
                    version.pack = 0;
                    version.build = dwBuild;
                }
                else
                {
                    version.major = rtlOSVersion.dwMajorVersion;
                    version.minor = rtlOSVersion.dwMinorVersion;
                    version.pack = rtlOSVersion.dwMajorVersion;
                    version.build = rtlOSVersion.dwBuildNumber;
                }
            }
        }

        return version;
    }

    std::string version_str(const winversion_t & ver)
    {
        if (ver >= winversion_10)
            return "10";
        else if (ver >= winversion_8_1)
            return "8.1";
        if (ver >= winversion_8)
            return "8";
        if (ver >= winversion_7)
            return "7";
        if (ver >= winversion_vista)
            return "Vista";
        if (ver >= winversion_server_2003)
            return "Server 2003";
        if (ver >= winversion_xp_sp3)
            return "XP SP3";
        if (ver >= winversion_xp_sp2)
            return "XP SP2";
        if (ver >= winversion_xp_sp1)
            return "XP SP1";
        if (ver >= winversion_xp)
            return "XP";
        if (ver >= winversion_2000)
            return "2000";
        return "Unknown";
    }

    drawing::font defaultFont()
    {
        NONCLIENTMETRICSW metrics = { sizeof(NONCLIENTMETRICSW) };
        SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, metrics.cbSize, &metrics, 0);

        drawing::font font;
        font.family = core::wstr_u8str(metrics.lfMessageFont.lfFaceName);
        font.size = float(metrics.lfMessageFont.lfHeight);
        return font;
    }

    LOGFONTW MappingFont(HDC hdc, const drawing::font & font)
    {
        LOGFONTW logfont = {};
        if (font.family[0])
            core::textcpy(logfont.lfFaceName, LF_FACESIZE, core::u8str_wstr(font.family).data(), -1);
        else
            core::textcpy(logfont.lfFaceName, LF_FACESIZE, core::u8str_wstr(win32::defaultFont().family).data(), -1);

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
