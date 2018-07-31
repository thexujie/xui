#pragma once

#include "Form.h"

namespace controls
{
    class Screen
    {
    public:
        Screen(void * handle);
        ~Screen();

        core::si32i size();
        core::rc32i rect();

    private:
        void * _handle = nullptr;
    };

    class Desktop
    {
    public:
        Desktop();
        ~Desktop();

        core::vec2i ppi() const;
        core::si32i size() const;
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
