#include "stdafx.h"
#include "View.h"
#include "Scene.h"
#include "Renderable.h"
#include "Interactable.h"

namespace ui::component
{
    void View::lock()
    {
        _mtx.lock();
    }

    void View::unlock()
    {
        auto s = scene();
        core::rc32f rect_invalid = _rect_invalid;
        _rect_invalid.clear();
        _mtx.unlock();

        if (!rect_invalid.empty() && s)
            s->invalid(rect_invalid);
    }

    void View::invalid_rect(const core::rc32f & rect)
    {
        _rect_invalid.unite(rect);
    }

    void View::enteringScene(std::shared_ptr<component::Scene> scene)
    {

    }

    void View::enterScene(std::shared_ptr<component::Scene> scene)
    {
        _scene = scene;
    }

    void View::leavingScene(std::shared_ptr<component::Scene> scene)
    {

    }

    void View::leaveScene(std::shared_ptr<component::Scene> scene)
    {
        _scene.reset();
    }


    void View::insert(std::shared_ptr<ui::component::Component> object)
    {
        assert(object);
        if (!object)
            throw core::exception(core::error_nullptr);

        {
            std::unique_lock<std::mutex> lock(_mtx, std::try_to_lock);
            switch (object->type())
            {
            case ComponentType::Renderable:
                _renderables.insert(std::make_pair(0, std::dynamic_pointer_cast<Renderable>(object)));
                invalid_rect(object->rect());
                break;
            case ComponentType::Interactable:
                _mouseareas.push_back(std::dynamic_pointer_cast<MouseArea>(object));
                break;
            default:
                break;
            }
        }
        object->enterView(std::dynamic_pointer_cast<View>(shared_from_this()));
    }

    void View::insert(int32_t depth, std::shared_ptr<Component> object)
    {
        assert(object);
        if (!object)
            throw core::exception(core::error_nullptr);

        switch (object->type())
        {
        case ComponentType::Renderable:
            _renderables.insert(std::make_pair(depth, std::dynamic_pointer_cast<Renderable>(object)));
            invalid_rect(object->rect());
            break;
        case ComponentType::Interactable:
            _mouseareas.push_back(std::dynamic_pointer_cast<MouseArea>(object));
            break;
        default:
            break;
        }
        object->enterView(std::dynamic_pointer_cast<View>(shared_from_this()));
    }

    void View::remove(std::shared_ptr<Component> object)
    {
        switch (object->type())
        {
        case ComponentType::Renderable:
            for(auto iter = _renderables.begin(); iter != _renderables.end(); ++iter)
            {
                if(iter->second == std::dynamic_pointer_cast<Renderable>(object))
                {
                    _renderables.erase(iter);
                    break;
                }
            }
            invalid_rect(object->rect());
            break;
        case ComponentType::Interactable:
            break;
        default:
            break;
        }
        object->leaveView(std::dynamic_pointer_cast<View>(shared_from_this()));
    }

    void View::clear()
    {
        _renderables.clear();
    }

    void View::render(graphics::Graphics & graphics, const graphics::Region & region) const
    {
        std::lock_guard<std::mutex> lock(const_cast<View *>(this)->_mtx);
        graphics.setMatrix(_transform);
        for(auto & rendereable : _renderables)
        {
            if(region.intersects(rendereable.second->rect().ceil<int32_t>()))
                rendereable.second->render(graphics);
        }
        graphics.setMatrix(core::float3x2::identity);
    }

    std::shared_ptr<MouseArea> View::findMouseArea(const core::pt32f & pos, std::shared_ptr<MouseArea> last) const
    {
        std::lock_guard<std::mutex> lock(const_cast<View *>(this)->_mtx_interactable);
        bool found = false;
        for (auto iter = _mouseareas.rbegin(); iter != _mouseareas.rend(); ++iter)
        {
            if (last && !found)
            {
                if(*iter == last)
                    found = true;
                continue;
            }

            if ((*iter)->onHitTest(pos) == core::error_ok)
                return *iter;
        }
        return nullptr;
    }
}
