#pragma once
#include "component/Scene.h"
#include "attribute.h"

namespace controls
{
    class Control;

    class View
    {
    public:
        View();
        virtual ~View();

        void setRect(const core::rc32f & rect);
        core::rc32f rect() const { return _rect; }
        void setPos(const core::pt32f & pos);
        core::pt32f pos() const { return _rect.pos; }
        void setSize(const core::si32f & size);
        core::si32f size() const { return _rect.size; }

        std::shared_ptr<component::Scene> scene() const;
        virtual void enteringScene(std::shared_ptr<component::Scene> & scene);
        virtual void enterScene(std::shared_ptr<component::Scene> & scene);
        virtual void leavingScene(std::shared_ptr<component::Scene> & scene);
        virtual void leaveScene(std::shared_ptr<component::Scene> & scene);

        void clear();
        void insert(int32_t depth, std::shared_ptr<controls::component::Renderable> renderable);

        protected:
        std::weak_ptr<component::Scene> _scene;
        core::rc32f _rect;
        std::multimap<int32_t, std::shared_ptr<controls::component::Renderable>> _renderables;
    };
}
