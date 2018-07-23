#pragma once
#include "Component.h"
#include "graphics/Graphics.h"

namespace controls::component
{
    class Renderable : public Component
    {
    public:
        Renderable();
        ~Renderable();

        virtual void render(graphics::Graphics & graphics) const;
    };
}
