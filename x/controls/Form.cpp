#include "stdafx.h"
#include "Form.h"
#include "win32/Window.h"
#include "Desktop.h"

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

        if(!_window)
        {
            auto window = std::make_shared<win32::Window>();
            window->attatch(std::dynamic_pointer_cast<Form>(shared_from_this()));
            _window = window;
        }
        _rect.size = calc(_size);
        layout();
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
}
