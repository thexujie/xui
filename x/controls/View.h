#pragma once
#include "component/Scene.h"
#include "attribute.h"

namespace controls
{
    class Control;

    class View :public component::Renderable
    {
    public:
        View(std::shared_ptr<Control> control);
        virtual ~View();

        void lock() ;
        void unlock();
        void invalid(const core::rc32f & rect);

        std::shared_ptr<component::Scene> scene() const { return _scene.lock(); }
        virtual void enteringScene(std::shared_ptr<component::Scene> & scene);
        virtual void enterScene(std::shared_ptr<component::Scene> & scene);
        virtual void leavingScene(std::shared_ptr<component::Scene> & scene);
        virtual void leaveScene(std::shared_ptr<component::Scene> & scene);

        void clear();
        void insert(int32_t depth, std::shared_ptr<controls::component::Renderable> renderable);

        void setTransform(const core::float3x2 & transform) { _transform = transform; }

        virtual void render(graphics::Graphics & graphics) const;
    protected:
        std::mutex _mtx;
        std::weak_ptr<component::Scene> _scene;
        std::weak_ptr<Control> _control;
        core::rc32f _rect;
        core::float3x2 _transform;
        std::multimap<int32_t, std::shared_ptr<controls::component::Renderable>> _renderables;

        core::rc32f _rect_invalid;
    };
}
