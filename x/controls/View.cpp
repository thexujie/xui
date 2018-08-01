#include "stdafx.h"
#include "View.h"

namespace controls
{
    View::View(std::shared_ptr<Control> control) : _control(control)
    {

    }
    View::~View()
    {

    }

    void View::lock()
    {
        _mtx.lock();
    }

    void View::unlock()
    {
        auto s = scene();
        if(!_rect_invalid.empty() && s)
        {
            s->invalid(_rect_invalid);
            _rect_invalid.clear();
        }

        _mtx.unlock();
    }

    void View::invalid(const core::rc32f & rect)
    {
        _rect_invalid = _rect_invalid.intersect(rect);
    }

    void View::enteringScene(std::shared_ptr<component::Scene> & scene)
    {

    }

    void View::enterScene(std::shared_ptr<component::Scene> & scene)
    {
        _scene = scene;
    }

    void View::leavingScene(std::shared_ptr<component::Scene> & scene)
    {

    }

    void View::leaveScene(std::shared_ptr<component::Scene> & scene)
    {
        _scene.reset();
    }

    void View::clear()
    {
        _renderables.clear();
    }

    void View::insert(int32_t depth, std::shared_ptr<controls::component::Renderable> renderable)
    {
        renderable->enterView(std::dynamic_pointer_cast<View>(shared_from_this()));
        _renderables.emplace(std::make_pair(depth, renderable));
        invalid(renderable->rect());
    }

    void View::render(graphics::Graphics & graphics) const
    {
        std::lock_guard<View> lock(const_cast<View &>(*this));
        graphics.setMatrix(_transform);
        for(auto & rendereable : _renderables)
        {
            rendereable.second->render(graphics);
        }
        graphics.setMatrix(core::float3x2::identity);
    }
}
