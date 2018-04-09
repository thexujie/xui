#include "stdafx.h"
#include "GraphicsService.h"
#include "Graphics.h"
#include "Image.h"
#include "win32/windows.h"

namespace win32
{
    std::shared_ptr<graphics::IGraphicsPixmap> GraphicsService::CreatePixmap(core::math::si32_t size)
    {
        if (!_hdc)
        {
            _hdc = std::make_shared<HDC>();
            HDC hdcScreen = GetDC(NULL);
            *_hdc = CreateCompatibleDC(hdcScreen);
            ::ReleaseDC(NULL, hdcScreen);
        }

        return std::make_shared<win32::Bitmap>(_hdc, size);
    }

    std::shared_ptr<graphics::IGraphicsImage> GraphicsService::CreateImage(std::string path)
    {
        if (!_hdc)
        {
            _hdc = std::make_shared<HDC>();
            HDC hdcScreen = GetDC(NULL);
            *_hdc = CreateCompatibleDC(hdcScreen);
            ::ReleaseDC(NULL, hdcScreen);
        }
        return std::make_shared<win32::Image>(_hdc, path);
    }

    std::shared_ptr<graphics::IGraphicsImage> GraphicsService::CreateString(std::string str, graphics::text::font font, core::color32 color)
    {
        return nullptr;
    }

    std::shared_ptr<graphics::IGraphics> GraphicsService::CreateGraphics(std::shared_ptr<core::Object> pixmap)
    {
        return std::make_shared<win32::Graphics>(std::dynamic_pointer_cast<win32::Bitmap>(pixmap));
    }
}
