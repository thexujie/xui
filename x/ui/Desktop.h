#pragma once

#include "Form.h"

namespace ui
{
    class Screen
    {
    public:
        Screen(void * handle);
        ~Screen();

        core::sizei size();
        core::recti rect();

    private:
        void * _handle = nullptr;
    };

    class Desktop
    {
    public:
        Desktop();
        ~Desktop();

        float32_t scale() const;
        core::vec2i ppi() const;
        core::sizei size() const;
        int32_t numScreens() const;
        std::shared_ptr<Screen> screen(int32_t index);

    private:
        void _enumScreens();

    public:
        static Desktop & instance();

    private:
        std::vector<std::shared_ptr<Screen>> _screens;
    };
}
