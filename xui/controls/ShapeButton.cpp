#include "stdafx.h"
#include "ShapeButton.h"

namespace xui::controls
{
    ShapeButton::ShapeButton()
    {
        _border = core::vec4<core::dimensionf>{ 1_px };
        _padding = {1_em, 0.5_em};
        _size = { 100_per , 100_per };
        _fixed_aspect = core::vec2{ 1.0f, 1.0f };
    }

    ShapeButton::ShapeButton(std::string text)
    {
        _border = core::vec4<core::dimensionf>{ 1_px };
        _padding = { 1_em, 0.5_em };
        _size = { 100_per, 100_per };
        _fixed_aspect = core::vec2{ 1.0f, 1.0f };
    }

    ShapeButton::~ShapeButton()
    {
        
    }

    void ShapeButton::propertyTableCallback(core::property_table & properties)
    {
        Control::propertyTableCallback(properties);
    }

    void ShapeButton::propertyTable(core::property_table & properties)
    {
        ShapeButton::propertyTableCallback(properties);
    }

    void ShapeButton::setPath(std::shared_ptr<drawing::Path> path)
    {
        _path = path;
    }

    std::string ShapeButton::styleName() const
    {
        if (_pressed)
            return "shapebutton:active";
        else if (_mousein)
            return "shapebutton:hover";
        else
            return "shapebutton";
    }

    void ShapeButton::draw(drawing::Graphics & graphics, const drawing::Region & region) const
    {
        auto b = box();
        std::lock_guard l(*this);
        _drawBackground(graphics);
        if(_path)
        {
            core::float3x2 matrix;
            //matrix *= core::float3x2::translate(-0.5f, -0.5f);
            matrix *= core::float3x2::scale(b.cx, b.cy);
            matrix *= core::float3x2::translate(b.x, b.y);
            drawing::Path path;
            _path->transform(matrix, path);
            //matrix *= core::float3x2::translate(box.x + box.cx * 0.5f, box.y + box.cy * 0.5f);
            //matrix *= core::float3x2::scale(box.cx, box.cy);
            //matrix *= core::float3x2::translate(box.x, box.y);
            graphics.drawPath(path, drawing::PathStyle().stoke(core::colors::Red, calc(1.5_px)));
        }
        _drawBorder(graphics);
    }
}
