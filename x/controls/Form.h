#pragma once
#include "Container.h"
#include "component/Window.h"

namespace controls
{
    class Form : public Container
    {
    public:
        Form();
        Form(core::vec2<core::dimensionf> & size);
        ~Form();

        void show();
        void centerScreen(int32_t screenIndex = 0);

    public:
        core::event<void(bool shown)> shownChanged;

    private:
        std::shared_ptr<component::Window> _window;
        bool _shown = false;
    };
}
