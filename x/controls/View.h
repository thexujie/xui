#pragma once
#include "component/Scene.h"
#include "attribute.h"

namespace controls
{
    class Control;

    class View :public component::Renderable
    {
    public:
        View();
        virtual ~View();

        std::shared_ptr<component::Scene> scene() const;
        virtual void enteringScene(std::shared_ptr<component::Scene> & scene);
        virtual void enterScene(std::shared_ptr<component::Scene> & scene);
        virtual void leavingScene(std::shared_ptr<component::Scene> & scene);
        virtual void leaveScene(std::shared_ptr<component::Scene> & scene);

        void clear();
        void insert(int32_t depth, std::shared_ptr<controls::component::Renderable> renderable);

        virtual void render(graphics::Graphics & graphics) const;
    protected:
        std::weak_ptr<component::Scene> _scene;
        core::rc32f _rect;
        std::multimap<int32_t, std::shared_ptr<controls::component::Renderable>> _renderables;
    };
}
