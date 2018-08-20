#include "stdafx.h"
#include "Form.h"
#include "win32/Window.h"
#include "Desktop.h"
#include "core/app.h"

namespace controls
{
    Form::Form()
    {
        _top_level = true;
    }

    Form::Form(core::vec2<core::dimensionf> & size)
    {
        _size = size;
        _top_level = true;
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
            windowPosChanged(pos_old, pos);
            //windowRectChanged(core::rc32f(pos_old, _rect_window.size), core::rc32f(pos, _rect_window.size));
        }
    }

    //void Form::setWindowSize(const core::si32f & size)
    //{
    //    auto size_old = _rect_window.size;
    //    if (size_old != size)
    //    {
    //        _rect_window.size = size;
    //        windowSizeChanged(size_old, size);
    //        windowRectChanged(core::rc32f(_rect_window.pos, size_old), core::rc32f(_rect_window.pos, _rect_window.size));
    //    }
    //}

    std::shared_ptr<controls::component::Scene> Form::formScene() const
    {
        auto s = scene();
        if (s)
            return s;

        if (!_form_scene)
            const_cast<Form*>(this)->_form_scene = std::make_shared<controls::component::Scene>(const_cast<Form*>(this)->share_ref<Control>());
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
        Control::arrange(rect, rect.size);

        if(!_window)
        {
            auto window = std::make_shared<win32::Window>();
            window->attatch(std::dynamic_pointer_cast<Form>(shared_from_this()));
            _window = window;
        }

        _shown = true;
        scene()->invalid(core::rc32f(core::pt32f(), realSize()));
        invoke([this]() {shownChanged(true); });
    }

    void Form::centerScreen(int32_t screenIndex)
    {
        auto rc = Desktop::instance().screen(screenIndex)->rect().to<float32_t>();
        auto s = calc(size());
        auto p = rc.leftTop() + (rc.size - s) * 0.5;
        setWindowPos(p);
    }

    void Form::enteringScene(std::shared_ptr<component::Scene> & scene)
    {
        Container::enteringScene(scene);
    }

    void Form::onSizeChanged(const core::si32f & from, const core::si32f & to)
    {
        Container::onSizeChanged(from, to);
    }

    void Form::onClose()
    {
        invoke([this]() {closed(); });
    }
}
