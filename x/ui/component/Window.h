#pragma once

namespace ui::component
{
    class Window : public core::object
    {
    public:
        Window();
        ~Window();

        virtual void move(const core::pt32f & pos) = 0;
        virtual void resize(const core::si32f & size) = 0;
    };
}