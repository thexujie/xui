#include "stdafx.h"
#include "Animation.h"

namespace ui::component
{
    void Animation::start()
    {
        for (auto a : animations)
            a->start();
    }

    void Animation::update()
    {
        for (auto a : animations)
            a->update();
    }
}
