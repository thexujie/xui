#include "stdafx.h"
#include "Form.h"
#include "win32/Window.h"
#include "Desktop.h"
#include "core/app.h"

namespace controls
{
    Form::Form()
    {
        
    }

    Form::Form(core::vec2<core::dimensionf> & size)
    {
        _size = size;
    }

    Form::~Form()
    {

    }

    void Form::show()
    {
        if (_shown)
            return;

        if(!_scene)
        {
            _scene = std::make_shared<controls::component::Scene>();
            enteringScene(_scene);
            enterScene(_scene);
        }

        auto rect = core::rc32f(calc(_pos), calc(_size));
        Control::layout(rect, rect.size);

        if(!_window)
        {
            auto window = std::make_shared<win32::Window>();
            window->attatch(std::dynamic_pointer_cast<Form>(shared_from_this()));
            _window = window;
        }

        update();
        scene()->invalid(realRect());

        _shown = true;
        shownChanged(true);
    }

    void Form::centerScreen(int32_t screenIndex)
    {
        auto rc = Desktop::instance().screen(screenIndex)->rect().to<float32_t>();
        auto s = calc(size());
        auto p = rc.leftTop() + (rc.size - s) * 0.5;
        setPos(p);
    }

    void Form::enteringScene(std::shared_ptr<component::Scene> & scene)
    {
        Container::enteringScene(scene);
    }

    void Form::onSizeChanged(const core::si32f & from, const core::si32f & to)
    {
        scene()->invalid(core::rc32f(core::pt32f(), realSize()));
        Container::onSizeChanged(from, to);
    }

    void Form::onClose()
    {
        core::app().quit(0);
    }
}
