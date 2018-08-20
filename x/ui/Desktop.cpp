#include "stdafx.h"
#include "Desktop.h"
#include "win32/windows.h"

namespace ui
{
    Screen::Screen(void * handle):_handle(handle)
    {
        
    }

    Screen::~Screen()
    {
        
    }

    core::si32i Screen::size()
    {
        HMONITOR hm = (HMONITOR)_handle;
        MONITORINFO mi = { sizeof(MONITORINFO) };
        ::GetMonitorInfoW(hm, &mi);
        return core::si32i(mi.rcMonitor.right - mi.rcMonitor.left,
            mi.rcMonitor.bottom - mi.rcMonitor.top);
    }

    core::rc32i Screen::rect()
    {
        HMONITOR hm = (HMONITOR)_handle;
        MONITORINFO mi = { sizeof(MONITORINFO) };
        ::GetMonitorInfoW(hm, &mi);
        return core::rc32i(mi.rcMonitor.left,
            mi.rcMonitor.top,
            mi.rcMonitor.right - mi.rcMonitor.left,
            mi.rcMonitor.bottom - mi.rcMonitor.top);
    }

    int32_t Desktop::numScreens() const
    {
        return _screens.size();
    }

    std::shared_ptr<Screen> Desktop::screen(int32_t index)
    {
        return _screens.at(index);
    }


    Desktop & Desktop::instance()
    {
        static std::shared_ptr<ui::Desktop> _instance =  std::make_shared<Desktop>();
        return *_instance;
    }

    Desktop::Desktop()
    {
        _enumScreens();
    }

    Desktop::~Desktop()
    {
        
    }

    core::vec2i Desktop::ppi() const
    {
        HDC hdc = GetDC(NULL);
        core::vec2i val(GetDeviceCaps(hdc, LOGPIXELSX), GetDeviceCaps(hdc, LOGPIXELSY));
        ReleaseDC(NULL, hdc);
        return val;
    }

    core::si32i Desktop::size() const
    {
        HWND hdesktop=::GetDesktopWindow();
        RECT rect = {};
        GetWindowRect(hdesktop, &rect);
        return core::si32i(rect.right, rect.bottom);
    }

    void Desktop::_enumScreens()
    {
        _screens.clear();

        const auto pfn = [](HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)->BOOL
        {
            Desktop * pthis = (Desktop  *)dwData;
            std::shared_ptr<Screen> screen = std::make_shared<Screen>(hMonitor);
            pthis->_screens.push_back(screen);
            return TRUE;
        };
        EnumDisplayMonitors(NULL, NULL, pfn, (LPARAM)this);
    }
}