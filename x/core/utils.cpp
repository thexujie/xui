#include "stdafx.h"
#include "utils.h"
#include "win32/windows.h"

namespace core
{
    void dbg_output(std::string str)
    {
        auto strw = core::u8str_wstr(str);
        strw.append(L"\r\n");
        ::OutputDebugStringW(strw.c_str());
    }
}
