#include "stdafx.h"
#include "Bitmap.h"

#include "skia/skia.h"
#include "win32/win32.h"

namespace drawing
{
    Bitmap::Bitmap(const core::sizei & size)
    {
        auto bitmap = std::make_shared<SkBitmap>();
        bitmap->allocN32Pixels(size.cx, size.cy);
        _native = bitmap;
    }

    bitmap_buffer Bitmap::buffer() const
    {
        if (!_native)
            return {};

        bitmap_buffer buffer = {};
        buffer.data = static_cast<byte_t *>(_native->getPixels());
        buffer.size = skia::to(_native->dimensions());
        buffer.strike = 4;
        buffer.pitch = static_cast<int32_t>(_native->rowBytes());
        return buffer;
    }

    core::sizei Bitmap::size() const
    {
        if (!_native)
            return {};
        return skia::to(_native->dimensions());
    }

    core::error Bitmap::Save(std::string path, image::image_type type, int32_t quality)
    {
        if (!_native)
            return core::error_nullptr;

        if (type == image::image_type_none)
            type = image::image_get_type_from_ext(std::filesystem::path(path).extension().string().c_str());

        SkFILEWStream stream(path.c_str());
        return SkEncodeImage(&stream, *_native, skia::from(type), quality) ? core::error_ok : core::error_inner;
    }

    Surface::Surface(const core::sizei & size)
    {
        resize(size);
    }

    Surface::~Surface()
    {
        if (_bmp)
        {
            ::DeleteObject(_bmp);
            _bmp = nullptr;
        }
        if(_hdc)
        {
            ::DeleteDC((HDC)_hdc);
            _hdc = nullptr;
        }
    }

    core::error Surface::resize(const core::sizei & size)
    {
        _native.reset();

        if (!_hdc)
        {
            HDC hdcScreen = ::GetDC(NULL);
            _hdc = ::CreateCompatibleDC(hdcScreen);
            ::ReleaseDC(NULL, hdcScreen);
        }

        int32_t strike = 4;
        int32_t pitch = size.cx * strike;
        if (pitch & 0x4)
            pitch += 0x4 - (pitch & 0x4);

        BITMAPINFO bmpInfo = { 0 };
        bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmpInfo.bmiHeader.biWidth = size.cx;
        bmpInfo.bmiHeader.biHeight = -size.cy;
        bmpInfo.bmiHeader.biPlanes = 1;
        bmpInfo.bmiHeader.biBitCount = (uint16_t)(strike << 3);
        bmpInfo.bmiHeader.biCompression = BI_RGB;
        bmpInfo.bmiHeader.biSizeImage = (uint32_t)(size.cy * pitch);

        void * data = nullptr;
        HBITMAP bmp = ::CreateDIBSection((HDC)_hdc, &bmpInfo, DIB_RGB_COLORS, &data, NULL, 0);
        if (!bmp)
            return core::error_inner;

        //if(_data)
        //{
        //    int32_t width = std::min(size.cx, _size.cx);
        //    int32_t height = std::min(size.cy, _size.cy);

        //    for(int32_t row = 0; row < height; ++row)
        //        memcpy((byte_t *)data + pitch * row, (byte_t *)_data + _pitch * row, width * 4);
        //}

        if (_bmp)
        {
            ::DeleteObject(_bmp);
            _bmp = nullptr;
        }

        ::SelectObject((HDC)_hdc, bmp);
        _size = size;
        _data = data;
        _strike = strike;
        _pitch = pitch;
        _bmp = bmp;
        _native = std::shared_ptr<SkSurface>(SkSurface::MakeRasterDirect(SkImageInfo::MakeN32Premul(size.cx, size.cy), data, pitch).release(), skia::skia_unref<SkSurface>);

        return core::error_ok;
    }

    bitmap_buffer Surface::buffer() const
    {
        if (!_native)
            return {};

        bitmap_buffer buffer = {};
        buffer.data = static_cast<byte_t *>(_data);
        buffer.size = _size;
        buffer.strike = _strike;
        buffer.pitch = _pitch;
        return buffer;
    }

    core::sizei Surface::size() const
    {
        return _size;
    }

    core::error Surface::Save(std::string path, image::image_type type, int32_t quality)
    {
        if (!_native)
            return core::error_nullptr;

        if (type == image::image_type_none)
            type = image::image_get_type_from_ext(std::filesystem::path(path).extension().string().c_str());

        SkBitmap bitmap;
        bitmap.allocN32Pixels(_size.cx, _size.cy);
        _native->readPixels(bitmap, 0, 0);
        SkFILEWStream stream(path.c_str());
        return SkEncodeImage(&stream, bitmap, skia::from(type), quality) ? core::error_ok : core::error_inner;
    }
}
