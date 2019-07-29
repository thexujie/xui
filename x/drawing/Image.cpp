#include "stdafx.h"
#include "Image.h"
#include "core/io/filestream.h"
#include "image/formats/bmp.h"
#include "image/formats/tga.h"

#include "skia/skia.h"
#include <SkImage.h>

namespace drawing
{
    using namespace core;

    Image::Image(const Image & another) :_native(another._native)
    {
        
    }

	Image::Image(const std::u8string & path)
	{
		sk_sp<SkData> data(SkData::MakeFromFileName(reinterpret_cast<const char *>(path.c_str())));
		_native.reset(SkImage::MakeFromEncoded(data).release(), [](SkImage * ptr) { if (ptr) SkSafeUnref(ptr); });
	}

    Image::~Image()
    {
    }

    core::error Image::Save(std::string path, image::image_type type, int32_t quality) const
    {
        if (!_native)
            return core::error_nullptr;

        if (type == image::image_type_none)
            type = image::image_get_type_from_ext(std::filesystem::path(path).extension().string().c_str());

        SkBitmap bitmap;
        _native->asLegacyBitmap(&bitmap);
        SkFILEWStream stream(path.c_str());
        return SkEncodeImage(&stream, bitmap, skia::from(type), quality) ? core::error_ok : core::error_inner;
    }

    int32_t Image::width() const
    {
        if (!_native)
            return 0;
        return _native->width();
    }

    int32_t Image::height() const
    {
        if (!_native)
            return 0;
        return _native->height();
    }

    core::sizei Image::size() const
    {
        if (!_native)
            return {};
        return { _native->width(), _native->height() };
    }

    float32_t Image::aspect() const
    {
        if (!_native || !_native->width() || !_native->height())
            return 1.0f;

        return  _native->width() * 1.0f / _native->height();
    }
}
