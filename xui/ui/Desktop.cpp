#include "stdafx.h"
#include "Desktop.h"
#include "x/platform/win32/win32.h"

namespace ui
{
    Screen::Screen(void * handle):_handle(handle)
    {
        
    }

    Screen::~Screen()
    {
        
    }

    core::sizei Screen::size()
    {
        HMONITOR hm = (HMONITOR)_handle;
        MONITORINFO mi = { sizeof(MONITORINFO) };
        ::GetMonitorInfoW(hm, &mi);
        return core::sizei(mi.rcMonitor.right - mi.rcMonitor.left,
            mi.rcMonitor.bottom - mi.rcMonitor.top);
    }

    core::recti Screen::rect()
    {
        HMONITOR hm = (HMONITOR)_handle;
        MONITORINFO mi = { sizeof(MONITORINFO) };
        ::GetMonitorInfoW(hm, &mi);
        return core::recti(mi.rcMonitor.left,
            mi.rcMonitor.top,
            mi.rcMonitor.right - mi.rcMonitor.left,
            mi.rcMonitor.bottom - mi.rcMonitor.top);
    }

    int32_t Desktop::numScreens() const
    {
        return int32_t(_screens.size());
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

    float32_t Desktop::scale() const
    {
        HDC hdc = GetDC(NULL);
        float32_t s = GetDeviceCaps(hdc, LOGPIXELSY) / 96.0f;
        ReleaseDC(NULL, hdc);
        return s;
    }

    core::vec2i Desktop::ppi() const
    {
        HDC hdc = GetDC(NULL);
        core::vec2i val(GetDeviceCaps(hdc, LOGPIXELSX), GetDeviceCaps(hdc, LOGPIXELSY));
        ReleaseDC(NULL, hdc);
        return val;
    }

    core::sizei Desktop::size() const
    {
        HWND hdesktop=::GetDesktopWindow();
        RECT rect = {};
        GetWindowRect(hdesktop, &rect);
        return core::sizei(rect.right, rect.bottom);
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