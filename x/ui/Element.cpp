#include "stdafx.h"
#include "Element.h"
#include "renderables/Image.h"
#include "renderables/Rectangle.h"
#include "renderables/Path.h"
#include "renderables/Line.h"
#include <combaseapi.h>

namespace ui
{
    Element::Element() { }
    Element::~Element() { }

    std::shared_ptr<component::View> Element::view() const
    {
        if (!_view)
            const_cast<std::shared_ptr<component::View> &>(_view) = std::make_shared<component::View>();
        return _view;
    }

    std::shared_ptr<component::Animation> Element::animation() const
    {
        if (!_animation)
            const_cast<std::shared_ptr<component::Animation> &>(_animation) = std::make_shared<component::Animation>();
        return _animation;
    }

    void Element::setLocation(const core::vec2f & pos)
    {
        auto pos_old = _rect.pos;
        if (pos_old != pos)
        {
            _rect.pos = pos;
            onLocationChanged(pos_old, pos);
            onRectChanged(core::rc32f(pos_old, _rect.size), core::rc32f(pos, _rect.size));
        }
    }

    void Element::setMeasure(const core::vec2f & size)
    {
        auto size_old = _rect.size;
        if (size_old != size)
        {
            _rect.size = size;
            onMeasureChanged(size_old, size);
            onRectChanged(core::rc32f(_rect.pos, size_old), core::rc32f(_rect.pos, _rect.size));
        }
    }

    void Element::setRect(const core::rc32f & rect)
    {
        auto rect_old = _rect;
        bool rectchanged = false;
        if (rect_old.pos != rect.pos)
        {
            _rect.pos = rect.pos;
            onLocationChanged(rect_old.pos, rect.pos);
            rectchanged = true;
        }
        if (rect_old.size != rect.size)
        {
            _rect.size = rect.size;
            onMeasureChanged(rect_old.size, rect.size);
            rectchanged = true;
        }
        if (rectchanged)
            onRectChanged(rect_old, rect);
    }

    core::rc32f Element::box() const
    {
        return core::rc32f(_rect.pos, _rect.size);
    }

    core::rc32f Element::borderBox() const
    {
        return core::rc32f(_rect.pos + _border.bleftTop(), _rect.size - _border.bsize());
    }

    core::rc32f Element::paddingBox() const
    {
        return core::rc32f(_rect.pos + _padding.bleftTop(), _rect.size - _padding.bsize());
    }

    core::rc32f Element::contentBox() const
    {
        return paddingBox();
    }

    core::rc32f Element::box(control_box cbox) const
    {
        switch (cbox)
        {
        case control_box::layout_box: return box();
        case control_box::border_box: return borderBox();
        case control_box::padding_box: return paddingBox();
        case control_box::content_box: return contentBox();
        default: return borderBox();
        }
    }

    std::array<core::pt32f, 4> Element::boderPoints(core::align edge) const
    {
        auto bbox = box();
        auto border = _border;
        switch (edge)
        {
        case core::align::left:
            return { bbox.leftBottom(), bbox.leftTop(),
            { bbox.left() + border.bleft * 0.5f, bbox.top() + border.btop * 0.5f },
            { bbox.left() + border.bleft * 0.5f, bbox.bottom() - border.bbottom * 0.5f } };
        case core::align::top:
            return { bbox.leftTop(), bbox.rightTop(),
            { bbox.right() - border.bright * 0.5f, bbox.top() + border.btop * 0.5f },
            { bbox.left() + border.bleft * 0.5f, bbox.top() + border.btop * 0.5f } };
        case core::align::right:
            return { bbox.rightTop(), bbox.rightBottom(),
            { bbox.right() - border.bright * 0.5f, bbox.bottom() - border.bbottom * 0.5f },
            { bbox.right() - border.bright * 0.5f, bbox.top() + border.btop * 0.5f } };
        case core::align::bottom:
            return { bbox.rightBottom(), bbox.leftBottom(),
            { bbox.left() + border.bleft * 0.5f, bbox.bottom() - border.bbottom * 0.5f },
            { bbox.right() - border.bright * 0.5f, bbox.bottom() - border.bbottom * 0.5f } };
        default:
            return {};
        }
    }

    std::array<core::pt32f, 2> Element::boderLine(core::align edge) const
    {
        auto bbox = box();
        auto border = _border;
        switch (edge)
        {
        case core::align::left:
            return { core::pt32f{ bbox.left() + border.bleft * 0.5f, bbox.bottom() },
                core::pt32f{ bbox.left() + border.bleft * 0.5f, bbox.top() } };
        case core::align::top:
            return { core::pt32f{ bbox.left(), bbox.top() + border.btop * 0.5f },
                core::pt32f{ bbox.right(), bbox.top() + border.btop * 0.5f } };
        case core::align::right:
            return { core::pt32f{ bbox.right() - border.bright * 0.5f, bbox.top() },
                core::pt32f{ bbox.right() - border.bright * 0.5f, bbox.bottom() } };
        case core::align::bottom:
            return { core::pt32f{ bbox.right(), bbox.bottom() - border.bbottom * 0.5f },
                core::pt32f{ bbox.left(), bbox.bottom() - border.bbottom * 0.5f } };
        default:
            return {};
        }
    }

    void Element::setBackgroundColor(core::color32 color)
    {
        if (color == _background_color)
            return;

        _background_color = color;
    }

    core::color32 Element::backgroundColor() const
    {
        return _background_color;
    }

    void Element::setBackgroundImage(std::shared_ptr<graphics::Image> image)
    {
        if (image == _background_image)
            return;
        _background_image = image;
    }

    std::shared_ptr<graphics::Image> Element::backgroundImage() const
    {
        return _background_image;
    }

    void Element::enteringScene(std::shared_ptr<component::Scene> & scene)
    {
        _scene = scene;
        scene->insert(view());
    }

    void Element::enterScene(std::shared_ptr<component::Scene> & scene)
    {
    }

    void Element::leavingScene(std::shared_ptr<component::Scene> & scene)
    {
        if (_view)
            scene->remove(_view);
        _scene.reset();
    }

    void Element::leaveScene(std::shared_ptr<component::Scene> & scene) { }


    void Element::onLocationChanged(const core::pt32f & from, const core::pt32f & to)
    {
        locationChanged(from, to);
    }

    void Element::onMeasureChanged(const core::si32f & from, const core::si32f & to)
    {
        measureChanged(from, to);
    }

    void Element::onRectChanged(const core::rc32f & from, const core::rc32f & to) { rectChanged(from, to); }

    void Element::_updateBackground(std::shared_ptr<component::View> & view)
    {
        if (_background_image)
        {
            if (_background_rect_obj)
            {
                view->remove(_background_rect_obj);
                _background_rect_obj = nullptr;
            }

            if (!_background_imgage_obj)
            {
                _background_imgage_obj = std::make_shared<renderables::Image>(_background_image);
                view->insert(DEPTH_BACKGROUND, _background_imgage_obj);
            }

            _background_imgage_obj->setRect(box(_background_box));
            if (!_background_size.empty())
                _background_imgage_obj->setImageSize(_background_size);
            _background_imgage_obj->setImageFitting(_background_fitting);
        }
        else if (_background_color && _background_color.visible())
        {
            if (_background_imgage_obj)
            {
                view->remove(_background_imgage_obj);
                _background_imgage_obj = nullptr;
            }
            if (!_background_rect_obj)
            {
                _background_rect_obj = std::make_shared<renderables::Rectangle>();
                view->insert(DEPTH_BACKGROUND, _background_rect_obj);
            }

            _background_rect_obj->setRect(box(_background_box));
            _background_rect_obj->setRectangle(box(_background_box));
            _background_rect_obj->setPathStyle(graphics::PathStyle().fill(_background_color));
        }
        else
        {
            if (_background_imgage_obj)
            {
                view->remove(_background_imgage_obj);
                _background_imgage_obj = nullptr;
            }
            if (_background_rect_obj)
            {
                view->remove(_background_rect_obj);
                _background_rect_obj = nullptr;
            }
        }
    }

    void Element::_updateBorder(std::shared_ptr<component::View> & view)
    {
        if (!_border.empty())
        {
            if (std::equal(_border.arr.begin() + 1, _border.arr.end(), _border.arr.begin()) &&
                std::equal(_border_colors.arr.begin() + 1, _border_colors.arr.end(), _border_colors.arr.begin()) &&
                std::equal(_border_styles.arr.begin() + 1, _border_styles.arr.end(), _border_styles.arr.begin()))
            {
                if (!_border_obj)
                {
                    _border_obj = std::make_shared<renderables::Rectangle>();
                    view->insert(DEPTH_FOREGROUND, _border_obj);
                }

                for (int32_t cnt = 0; cnt < 4; ++cnt)
                {
                    auto & border_obj = _border_objs[cnt];
                    if (border_obj)
                    {
                        view->remove(border_obj);
                        border_obj = nullptr;
                    }
                }
                _border_obj->setRect(box());
                _border_obj->setRectangle(box().expanded(_border * -0.5f));
                _border_obj->setPathStyle(graphics::PathStyle().stoke(_border_colors.x, _border_styles[0]).width(_border.x));
            }
            else
            {
                if (_border_obj)
                {
                    view->remove(_border_obj);
                    _border_obj = nullptr;
                }

                auto border = _border;
                const core::align edges[4] = { core::align::left, core::align::top, core::align::right, core::align::bottom };
                for (int32_t cnt = 0; cnt < 4; ++cnt)
                {
                    auto & border_obj = _border_objs[cnt];
                    if (border[cnt] > 0 && _border_colors[cnt].visible())
                    {
                        auto path = std::make_shared<graphics::Path>();
                        auto points = boderPoints(edges[cnt]);
                        auto line = boderLine(edges[cnt]);
                        path->fromPoints(std::begin(points), std::end(points), true);

                        if (!border_obj)
                        {
                            border_obj = std::make_shared<renderables::Line>(line[0], line[1]);
                            view->insert(DEPTH_FOREGROUND, border_obj);
                        }
                        border_obj->setRect(path->computeTightBounds());
                        border_obj->setClipPath(path);
                        border_obj->setPathStyle(graphics::PathStyle().stoke(_border_colors[cnt], _border_styles[cnt]).width(border.arr[cnt]));
                    }
                    else
                    {
                        if (border_obj)
                        {
                            view->remove(border_obj);
                            border_obj = nullptr;
                        }
                    }
                }
            }
        }
    }

    void Element::_adjustSizeMinMax(core::si32f & size) const
    {
        if (!_minSize.empty())
        {
            if (_minSize.cx > 0)
            {
                float32_t val = _minSize.cx;
                if (size.cx < val)
                    size.cx = val;
            }
            if (_minSize.cy > 0)
            {
                float32_t val = _minSize.cy;
                if (size.cy < val)
                    size.cy = val;
            }
        }

        if (!_maxSize.empty())
        {
            if (_maxSize.cx > 0)
            {
                float32_t val = _maxSize.cx;
                if (size.cx > val)
                    size.cx = val;
            }
            if (_maxSize.cy > 0)
            {
                float32_t val = _maxSize.cy;
                if (size.cy > val)
                    size.cy = val;
            }
        }
    }
}
