#pragma once
#include "ui/Control.h"

namespace ui::base
{
    class Button : public Control
    {
    public:
        Button();
        ~Button();

        static void propertyTableCallback(core::property_table & properties);
        void propertyTable(core::property_table & properties) override;

        std::string styleName() const override;
    };
}
