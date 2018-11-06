#pragma once

namespace ui::component
{
    class Window : public core::object
    {
    public:
        Window();
        ~Window();

        virtual void move(const core::pointf & pos) = 0;
        virtual void resize(const core::sizef & size) = 0;
    };
}