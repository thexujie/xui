#include "stdafx.h"
#include "Control.h"
#include "renderables/Image.h"
#include "renderables/Rectangle.h"
#include "renderables/Path.h"
#include "renderables/Line.h"

namespace controls
{
    Control::Control() { }
    Control::~Control() { }

    core::si32f Control::prefferSize() const
    {
        // 如果设置了固定大小，直接返回即可
        if (_size.aviliable() && _size.value.cx.avi() && _size.value.cy.avi())
            return calc(_size.value) + calc(_padding).bsize();

        core::si32f size = contentSize() + calc(_padding).bsize();
        if (_size.aviliable())
        {
            if (_size.value.cx.avi())
                size.cx = calc(_size.value.cx);
            else if (_size.value.cy.avi())
                size.cy = calc(_size.value.cy);
            else {}
        }

        _adjustSizeMinMax(size);
        return size;
    }

    std::shared_ptr<View> Control::view() const
    {
        if (!_view)
            const_cast<std::shared_ptr<View> &>(_view) = std::make_shared<View>(const_cast<Control *>(this)->shared_from_this());
        return _view;
    }

    const core::color32 & Control::color() const
    {
        if (_color.aviliable())
            return _color;

        auto p = parent();
        if (p)
            return p->color();
        else
            return _color;
    }

    const graphics::font & Control::font() const
    {
        if (_font.aviliable())
            return _font;

        auto p = parent();
        if (p)
            return p->font();
        else
            return _font;
    }

    float32_t Control::calc(const core::dimensionf & value) const
    {
        auto s = scene();
        auto p = parent();
        if (!s)
            throw core::error_state;

        switch (value.unit)
        {
        case core::unit::px:
            return value.value * s->ratio();
        case core::unit::em:
            return value.value * font().size * s->ratio();
        case core::unit::pt:
            return value.value * 72.0f * s->ratio();
        case core::unit::dot:
            return value.value;
        case core::unit::per:
            if (!p)
                throw core::error_state;
            return value.value * p->width();
        default:
            return value.value * s->ratio();
        }
    }

    core::vec2f Control::calc(const core::vec2<core::dimensionf> & value) const
    {
        auto s = scene();
        auto p = parent();
        if (!s)
            throw core::error_state;

        float32_t x = 0;
        float32_t y = 0;
        switch (value.x.unit)
        {
        case core::unit::px:
            x = value.x.value * s->ratio();
            break;
        case core::unit::em:
            x = value.x.value * font().size * s->ratio();
            break;
        case core::unit::pt:
            x = value.x.value * 72.0f * s->ratio();
            break;
        case core::unit::dot:
            x = value.x.value;
            break;
        case core::unit::per:
            if (!p)
                throw core::error_state;
            x = value.x.value * p->width();
            break;
        default:
            x = value.x.value * s->ratio();
            break;
        }

        switch (value.y.unit)
        {
        case core::unit::px:
            y = value.y.value * s->ratio();
            break;
        case core::unit::em:
            y = value.y.value * font().size * s->ratio();
            break;
        case core::unit::pt:
            y = value.y.value * 72.0f * s->ratio();
            break;
        case core::unit::dot:
            y = value.y.value;
            break;
        case core::unit::per:
            if (!p)
                throw core::error_state;
            y = value.y.value * p->height();
            break;
        default:
            y = value.y.value * s->ratio();
            break;
        }

        return { x, y };
    }

    core::vec4f Control::calc(const core::vec4<core::dimensionf> & value) const
    {
        return { calc(value.xy), calc(value.zw) };
    }

    void Control::setPos(const core::vec2f & pos)
    {
        auto pos_old = _rect.pos;
        if (pos_old != pos)
        {
            _rect.pos = pos;
            onPosChanged(pos_old, pos);
            onRectChanged(core::rc32f(pos_old, _rect.size), core::rc32f(pos, _rect.size));
        }
    }

    void Control::setSize(const core::vec2f & size)
    {
        auto size_old = _rect.size;
        if (size_old != size)
        {
            _rect.size = size;
            onSizeChanged(size_old, size);
            onRectChanged(core::rc32f(_rect.pos, size_old), core::rc32f(_rect.pos, _rect.size));
        }
    }

    core::rc32f Control::box() const
    {
        return core::rc32f(_rect.pos, _rect.size);
    }

    core::rc32f Control::borderBox() const
    {
        return core::rc32f(_rect.pos + calc(_border).bleftTop(), _rect.size - calc(_border).bsize());
    }

    core::rc32f Control::paddingBox() const
    {
        return core::rc32f(_rect.pos + calc(_padding).bleftTop(), _rect.size - calc(_padding).bsize());
    }

    core::rc32f Control::contentBox() const
    {
        return paddingBox();
    }

    core::rc32f Control::box(control_box box) const
    {
        switch (box)
        {
        case control_box::border_box: return borderBox();
        case control_box::padding_box: return paddingBox();
        case control_box::content_box: return contentBox();
        default: return borderBox();
        }
    }

    void Control::invalid() { }

    std::array<core::pt32f, 4> Control::boderPoints(core::align edge) const
    {
        auto bbox = box();
        auto border = calc(_border);
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

    std::array<core::pt32f, 2> Control::boderLine(core::align edge) const
    {
        auto bbox = box();
        auto border = calc(_border);
        switch (edge)
        {
        case core::align::left:
            return {core::pt32f{ bbox.left() + border.bleft * 0.5f, bbox.bottom() },
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

    void Control::setBackgroundColor(core::color32 color)
    {
        if (color == _background_color)
            return;

        _background_color = color;
        if (!_background_image)
            invalid();
    }

    core::color32 Control::backgroundColor() const
    {
        return _background_color;
    }

    void Control::setBackgroundImage(std::shared_ptr<graphics::Image> image)
    {
        if (image == _background_image)
            return;
        _background_image = image;
        invalid();
    }

    std::shared_ptr<graphics::Image> Control::backgroundImage() const
    {
        return _background_image;
    }

    void Control::enteringScene(std::shared_ptr<component::Scene> & scene)
    {
        _scene = scene;
        scene->addRenderable(view());
    }

    void Control::enterScene(std::shared_ptr<component::Scene> & scene)
    {
    }

    void Control::leavingScene(std::shared_ptr<component::Scene> & scene)
    {
        if (_view)
            scene->removeRenderable(_view);
        _scene.reset();
    }

    void Control::leaveScene(std::shared_ptr<component::Scene> & scene) { }

    void Control::layout(const core::rc32f & rect, const core::si32f & size)
    {
        auto p = parent();
        _rect.pos = rect.pos;
        _rect.size = size;
        layout();
        update();
    }

    void Control::update()
    {
        auto v = view();
        std::lock_guard<View> lock(*v);
        v->clear();
        _updateBackground(v);
        updateContent(v);
        _updateBorder(v);
    }

    void Control::onPosChanged(const core::pt32f & from, const core::pt32f & to) { posChanged(from, to); }
    void Control::onSizeChanged(const core::si32f & from, const core::si32f & to)
    {
        update();
        sizeChanged(from, to);
    }
    void Control::onRectChanged(const core::rc32f & from, const core::rc32f & to) { rectChanged(from, to); }

    void Control::_updateBackground(std::shared_ptr<View> & view) const
    {
        if (_background_image)
        {
            auto image = std::make_shared<renderables::Image>(_background_image);
            image->setRect(box(_background_box));
            if (_background_size.aviliable())
                image->setImageSize(calc(_background_size));
            image->setImageFitting(_background_fitting);
            view->insert(0, image);
        }
        else if (_background_color.visible())
        {
            auto rectangle = std::make_shared<renderables::Rectangle>();
            rectangle->setRect(box(_background_box));
            rectangle->setPathStyle(graphics::PathStyle().fill(_background_color));
            view->insert(0, rectangle);
        }
        else {}
    }

    void Control::_updateBorder(std::shared_ptr<View> & view) const
    {
        if (_border && _border_colors)
        {
            if(std::equal(_border.value.arr.begin() + 1, _border.value.arr.end(), _border.value.arr.begin()) &&
                std::equal(_border_colors.value.arr.begin() + 1, _border_colors.value.arr.end(), _border_colors.value.arr.begin()) &&
                std::equal(_border_styles.value.arr.begin() + 1, _border_styles.value.arr.end(), _border_styles.value.arr.begin()))
            {
                auto item = std::make_shared<renderables::Rectangle>();
                item->setRect(box().expand(calc(_border) * -0.5f));
                item->setPathStyle(graphics::PathStyle().stoke(_border_colors.value.x, _border_styles.value[0]).width(calc(_border.value.x)));
                view->insert(0, item);
            }
            else
            {
                auto border = calc(_border);
                const core::align edges[4] = { core::align::left, core::align::top, core::align::right, core::align::bottom };
                for(int32_t cnt = 0; cnt < 4; ++cnt)
                {
                    if (border[cnt] > 0 && _border_colors.value[cnt].visible())
                    {
                        auto path = std::make_shared<graphics::Path>();
                        auto points = boderPoints(edges[cnt]);
                        auto line = boderLine(edges[cnt]);
                        path->fromPoints(std::begin(points), std::end(points), true);

                        auto item = std::make_shared<renderables::Line>(line[0], line[1]);
                        item->setClipPath(path);
                        item->setPathStyle(graphics::PathStyle().stoke(_border_colors.value[cnt], _border_styles.value[cnt]).width(border.arr[cnt]));
                        view->insert(0, item);
                    }
                }
            }
        }
    }

    void Control::_adjustSizeMinMax(core::si32f & size) const
    {
        if (_minSize.aviliable())
        {
            if (_minSize.value.cx.avi())
            {
                float32_t val = calc(_minSize.value.cx);
                if (size.cx < val)
                    size.cx = val;
            }
            if (_minSize.value.cy.avi())
            {
                float32_t val = calc(_minSize.value.cy);
                if (size.cy < val)
                    size.cy = val;
            }
        }

        if (_maxSize.aviliable())
        {
            if (_maxSize.value.cx.avi())
            {
                float32_t val = calc(_maxSize.value.cx);
                if (size.cx > val)
                    size.cx = val;
            }
            if (_maxSize.value.cy.avi())
            {
                float32_t val = calc(_maxSize.value.cy);
                if (size.cy > val)
                    size.cy = val;
            }
        }
    }
}
