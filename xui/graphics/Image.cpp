#include "stdafx.h"
#include "Image.h"
#include "core/io/filestream.h"
#include "image/formats/bmp.h"
#include "image/formats/tga.h"

namespace graphics
{
    using namespace core;

    Image::~Image()
    {
        if (_img.data.data)
            _img.pfn_free(_img.data);
    }

    Image::Image(std::string path)
    {
        //_image = GraphicsService().CreateImage(path);
        auto[data, size] = core::io::readFullFile(path);
        if (data)
            graphics::image::image_create(data.get(), (int32_t)size, _img);
    }

    si32_t Image::size() const
    {
        return { _img.data.format.width, _img.data.format.height };
    }

    core::error_e Image::Save(std::string path) const
    {
        if (!_img.data.data)
            return error_state;

        if (path.rfind(".bmp") != std::string::npos)
            return image::formats::bmp_save(_img.data, path);
        else if (path.rfind(".tga") != std::string::npos)
            return image::formats::tga_save(_img.data, path);
        return error_not_supported;
    }
}
