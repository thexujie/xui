#include "stdafx.h"
#include "Form.h"
#include "win32/Window.h"
#include "Desktop.h"
#include "core/app.h"

namespace ui
{
    Form::Form(form_styles styles) :_styles(styles)
    {
        _background_color = core::colors::AliceBlue;
        _mouse_through = false;
    }

    Form::Form(core::vec2<core::dimensionf> & size, form_styles styles) : _styles(styles)
    {
        _background_color = core::colors::AliceBlue;
        _size = size;
        _mouse_through = false;
    }

    Form::~Form()
    {

    }

    void Form::setWindowPos(const core::pt32f & pos)
    {
        auto pos_old = _rect_window.pos;
        if (pos_old != pos)
        {
            _rect_window.pos = pos;
            if (_window)
                _window->move(pos);
            windowPosChanged(pos_old, pos);
            //windowRectChanged(core::rc32f(pos_old, _rect_window.size), core::rc32f(pos, _rect_window.size));
        }
    }

    void Form::setWindowSize(const core::si32f & size)
    {
        setShowSize(size);
    }

    std::shared_ptr<Scene> Form::formScene() const
    {
        auto s = scene();
        if (s)
            return s;

        if (!_form_scene)
            const_cast<Form*>(this)->_form_scene = std::make_shared<Scene>(const_cast<Form*>(this)->share_ref<Control>());
        return _form_scene;
    }

    void Form::show()
    {
        if (_shown)
            return;

        if (!scene())
        {
            auto s = formScene();
            enteringScene(s);
            enterScene(s);
        }

        auto rect = core::rc32f(calc(_pos), calc(_size));
        Container::place(rect, rect.size);

        if(!_window)
        {
            auto window = std::make_shared<win32::Window>();
            window->attatch(share_ref<Form>());
            _window = window;
        }

        _shown = true;
        invalidate(core::rc32f(core::pt32f(), realSize()));
        shownChanged(true);
    }

    void Form::centerScreen(int32_t screenIndex)
    {
        auto rc = Desktop::instance().screen(screenIndex)->rect().to<float32_t>();
        auto s = calc(size());
        auto p = rc.leftTop() + (rc.size - s) * 0.5;
        setWindowPos(p);
    }

    void Form::invalidate(const core::rc32f & rect)
    {
        if(_form_scene)
            _form_scene->invalid(rect.intersected(core::rc32f(core::pt32f(), realSize())));
    }

    std::shared_ptr<Control> Form::findChild(const core::pt32f & pos, std::shared_ptr<Control> last, findchild_flags flags) const
    {
        auto child = Container::findChild(pos, last, flags);
        return child ? child : control_ref();
    }

    void Form::enteringScene(std::shared_ptr<Scene> & scene)
    {
        Container::enteringScene(scene);
    }

    void Form::onSizeChanged(const core::si32f & from, const core::si32f & to)
    {
        if (_window)
            _window->resize(to);
        Container::onSizeChanged(from, to);
    }

    void Form::ondraw(drawing::Graphics & graphics, const drawing::Region & region) const
    {
        Container::ondraw(graphics, region);
    }

    form_hittest Form::hitTestForm(const core::pt32f & pos) const
    {
        if (!_rect.contains(pos))
            return form_hittest::nowhere;

        if(!_resize_borders)
            return form_hittest::client;

        enum
        {
            left = 0x0001,
            right = 0x0002,
            top = 0x0004,
            bottom = 0x0008,
        };

        auto b = calc(_resize_borders.value);
        uint32_t temp = 0;
        if (pos.x < b.bleft)
            temp |= left;
        else if (pos.x >= _rect.cx - b.bright)
            temp |= right;
        else {}
        if (pos.y < b.btop)
            temp |= top;
        else if (pos.y >= _rect.cy - b.bbottom)
            temp |= bottom;
        else {}

        switch (temp)
        {
        case left | top: return form_hittest::resize_leftTop;
        case top: return form_hittest::resize_top;
        case right | top: return form_hittest::resize_rightTop;
        case right: return form_hittest::resize_right;
        case right | bottom: return form_hittest::resize_rightBottom;
        case bottom: return form_hittest::resize_bottom;
        case left | bottom: return form_hittest::resize_leftBottom;
        case left: return form_hittest::resize_left;
        default: break;
        }

        auto child = findChild(pos);
        if (!child)
            return form_hittest::client;

        auto ht = child->hitTest(pos);
        switch (ht)
        {
        case ui::hittest_result::nowhere: return form_hittest::nowhere;
        case ui::hittest_result::client: return form_hittest::client;
        case ui::hittest_result::stable: return form_hittest::caption;
        case ui::hittest_result::transparent: return form_hittest::caption;
        default: return form_hittest::client;
        }
    }

    void Form::onClose()
    {
        invoke([this]() {closed(); });
    }
}
