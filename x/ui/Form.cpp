#include "stdafx.h"
#include "Form.h"
#include "win32/Window.h"
#include "Desktop.h"
#include "core/app.h"

namespace ui
{
    Form::Form(form_styles styles) :_styles(styles)
    {
        _mouse_through = false;
    }

    Form::Form(core::vec2<core::dimensionf> & size, form_styles styles) : _styles(styles)
    {
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
        invalid(core::rc32f(core::pt32f(), realSize()));
        shownChanged(true);
    }

    void Form::centerScreen(int32_t screenIndex)
    {
        auto rc = Desktop::instance().screen(screenIndex)->rect().to<float32_t>();
        auto s = calc(size());
        auto p = rc.leftTop() + (rc.size - s) * 0.5;
        setWindowPos(p);
    }

    void Form::invalid(const core::rc32f & rect)
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
        if (_styles.all(form_style::layered))
            graphics.clear(0);
        Container::ondraw(graphics, region);
    }

    void Form::onClose()
    {
        invoke([this]() {closed(); });
    }
}
