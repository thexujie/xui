#include "stdafx.h"
#include "Image.h"
#include "core/io/filestream.h"
#include "image/formats/bmp.h"
#include "image/formats/tga.h"

#include "skia/skia.h"
#include <SkImage.h>

namespace graphics
{
    using namespace core;

    Image::~Image()
    {
    }

    Image::Image(std::string path)
    {
        sk_sp<SkData> data(SkData::MakeFromFileName(path.c_str()));
        _native.reset(SkImage::MakeFromEncoded(data).release(), [](SkImage * ptr) { if(ptr) SkSafeUnref(ptr); });
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
        return SkEncodeImage(&stream, bitmap, skia::to(type), quality) ? core::error_ok : core::error_inner;
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

    core::si32i Image::size() const
    {
        if (!_native)
            return {};
        return { _native->width(), _native->height() };
    }
}
