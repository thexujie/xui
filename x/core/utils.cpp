#include "stdafx.h"
#include "utils.h"
#include "win32/windows.h"

namespace core
{
    void dbg_output(std::string str)
    {
        auto strw = core::string::u8_u16(str);
        strw.append(L"\r\n");
        ::OutputDebugStringW(strw.c_str());
    }
}
