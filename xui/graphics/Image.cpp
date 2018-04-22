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
        if (_image.data)
            _image.pfn_free(_image);
    }

    Image::Image(std::string path)
    {
        //_image = GraphicsService().CreateImage(path);
        auto[data, size] = core::io::readFullFile(path);
        if (data)
            graphics::image::image_create(data.get(), (int32_t)size, _image);
    }

    core::error_e Image::Save(std::string path) const
    {
        if (!_image.data)
            return error_state;

        if (path.rfind(".bmp") != std::string::npos)
            return image::formats::bmp_save(_image, path);
        else if (path.rfind(".tga") != std::string::npos)
            return image::formats::tga_save(_image, path);
        else
            return image::formats::bmp_save(_image, path);
    }
}
