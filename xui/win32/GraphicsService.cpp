#include "stdafx.h"
#include "GraphicsService.h"
#include "Graphics.h"
#include "Image.h"
#include "win32/windows.h"

namespace win32
{
    std::shared_ptr<graphics::IPixmap> GraphicsService::CreatePixmap(core::math::si32_t size)
    {
        return std::dynamic_pointer_cast<graphics::IPixmap>(std::make_shared<win32::Bitmap>(size));
    }

    std::shared_ptr<graphics::IGraphics> GraphicsService::CreateGraphics(std::shared_ptr<core::Object> pixmap)
    {
        return std::make_shared<win32::Graphics>(std::dynamic_pointer_cast<win32::Bitmap>(pixmap));
    }

    std::shared_ptr<graphics::IImage> GraphicsService::CreateImage(std::string path)
    {
        if(!_hdc)
        {
            HDC hdcScreen = GetDC(NULL);
            _hdc = CreateCompatibleDC(hdcScreen);
            ::ReleaseDC(NULL, hdcScreen);
        }
        return std::make_shared<win32::Image>(_hdc, path);
    }
}
