#pragma once
#include "component/Scene.h"
#include "attribute.h"

namespace controls
{
    class Control
    {
    public:
        Control();
        virtual ~Control();

        void setRect(const core::rc32f & rect);
        core::rc32f rect() const { return _rect; }
        void setPos(const core::pt32f & pos);
        core::pt32f pos() const { return _rect.position; }
        void setSize(const core::si32f & size);
        core::si32f size() const { return _rect.size; }

        std::shared_ptr<component::Scene> scene() const;
        virtual void enteringScene(std::shared_ptr<component::Scene> & scene);
        virtual void enterScene(std::shared_ptr<component::Scene> & scene);
        virtual void leavingScene(std::shared_ptr<component::Scene> & scene);
        virtual void leaveScene(std::shared_ptr<component::Scene> & scene);

        virtual void onRectChanged(const core::rc32f & from, const core::rc32f & to);

    public:
        core::event<void(const core::rc32f & from, const core::rc32f & to)> rectChanged;

    protected:
        std::weak_ptr<component::Scene> _scene;
        core::rc32f _rect;

        core::float4 _border;
        core::float2 _padding;
        attribute<core::color32> _backgroundColor;
    };
}
