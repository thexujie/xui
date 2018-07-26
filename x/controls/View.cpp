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
        _renderables.emplace(std::make_pair(depth, renderable));
    }

    void View::render(graphics::Graphics & graphics) const
    {
        for(auto & rendereable : _renderables)
        {
            rendereable.second->render(graphics);
        }
    }
}
