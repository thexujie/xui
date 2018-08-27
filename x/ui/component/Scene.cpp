#include "stdafx.h"
#include "Component.h"
#include "Scene.h"
#include "ui/Control.h"

namespace ui::component
{
    Scene::Scene(std::shared_ptr<Control> control) : _control(control)
    {
        
    }

    Scene::~Scene()
    {
        _exit = true;
        if(_th_render.joinable())
        {
            _cv_render.notify_all();
            _th_render.join();
        }

        if (_future.valid())
            _future.wait();
    }

    void Scene::invalid(const core::rc32f & rect)
    {
        {
            std::lock_guard<std::mutex> lock(_mtx);
            auto rc = rect.ceil<int32_t>();
            _invalid_rect.unite(rc);
            _invalid_region.addRect(rc);
        }

        if(!_future.valid() || _future.wait_for(0s) == std::future_status::ready)
            _future = std::async(std::bind(&Scene::renderThread, this));
    }

    void Scene::update()
    {
        std::lock_guard<std::mutex> lock(_mtx);
        flush();
    }

    void Scene::flush()
    {
    }

    core::error Scene::insert(std::shared_ptr<Control> control)
    {
        if (!control)
            return core::error_args;

        std::lock_guard<std::mutex> lock(_mtx);
        _controls.insert(std::make_pair(control->ZValue(), control));
        return core::error_ok;
    }

    core::error Scene::remove(std::shared_ptr<Control> control)
    {
        if (!control)
            return core::error_args;

        auto s = share_ref<Scene>();
        std::lock_guard<std::mutex> lock(_mtx);
        for (auto iter = _controls.begin(); iter != _controls.end(); )
        {
            if (iter->second == control)
            {
                control->setParent(nullptr);
                _controls.erase(iter);
                break;
            }
        }
        return core::error_ok;
    }

    core::error Scene::start(std::shared_ptr<Control> control)
    {
        if (std::find_if(_animations.begin(), _animations.end(), [&control](const std::pair<core::flags, std::shared_ptr<Control>> & pair) { return pair.second == control; }) != _animations.end())
            return core::error_ok;

        _animations.push_back(std::make_pair(core::flag::none, control));
        if(!_animation_timer.running())
        {
            _animation_timer.tick += std::bind(&Scene::animationTimerTick, this, std::placeholders::_1, std::placeholders::_2);
            _animation_timer.start(1000ms / 60);
        }
        return core::error_ok;
    }

    std::shared_ptr<MouseArea> Scene::findMouseArea(const core::pt32f & pos, std::shared_ptr<MouseArea> last) const
    {
        for (auto iter = _controls.rbegin(); iter != _controls.rend(); ++iter)
        {
            auto ma = iter->second->findMouseArea(pos, last);
            if (ma)
                return ma;
        }
        return nullptr;
    }

    void Scene::onMouseEnter(const mosue_state & state)
    {
        _updateMouseArea(state);
    }

    void Scene::onMouseMove(const mosue_state & state)
    {
        _updateMouseArea(state);
        if (_mousearea_current)
            _mousearea_current->onMouseMove(state);
    }

    void Scene::onMouseLeave(const mosue_state & state)
    {
        _updateMouseArea(state);
    }

    void Scene::onMouseDown(const mosue_state & state)
    {
        if (_mousearea_current)
        {
            if (_mousearea_current->captureButtons())
                captured(true);
            _mousearea_current->onMouseDown(state);
        }
    }

    void Scene::onMouseUp(const mosue_state & state)
    {
        if (_mousearea_current)
        {
            _mousearea_current->onMouseUp(state);
            if (_mousearea_current->captureButtons())
                captured(false);
        }

        _updateMouseArea(state);
    }

    void Scene::onMouseClick(const mosue_state & state)
    {
        if (_mousearea_current)
            _mousearea_current->onMouseClick(state);
    }

    void Scene::onMouseDBClick(const mosue_state & state)
    {
        if (_mousearea_current)
            _mousearea_current->onMouseDBClick(state);
    }

    void Scene::onMouseState(const mosue_state & state)
    {
        if(state.action() == mouse_action::v_wheeling)
        {
            //findMouseArea(state.pos());
            if (_mousearea_current && _mousearea_current->acceptWheelV())
                _mousearea_current->onMouseWheel(state);
            else
            {
                auto obj = _mousearea_current;
                while(true)
                {
                    obj = findMouseArea(state.pos(), obj);
                    if (!obj)
                        break;

                    if (!obj->acceptWheelV())
                        continue;

                    obj->onMouseWheel(state);
                    break;
                }
            }
            _updateMouseArea(state);
        }
    }

    void Scene::_updateMouseArea(const mosue_state & state)
    {
        auto ma = state.action() == mouse_action::leaving ?  nullptr : findMouseArea(state.pos());
        // do not update while capturing one or more button(s)
        if (_mousearea_current != ma &&
            (!_mousearea_current || !(_mousearea_current->captureButtons() & state.buttons())))
        {
            if (_mousearea_current)
                _mousearea_current->onMouseLeave(state);

            _mousearea_current = ma;

            if (_mousearea_current)
                _mousearea_current->onMouseEnter(state);
        }
    }

    void Scene::renderThread()
    {
        //std::unique_lock<std::mutex> lock(_mtx);

        core::rc32i invalid_rect;
        graphics::Region invalid_region;
        {
            std::lock_guard<std::mutex> lock(_mtx);
            invalid_rect = std::move(_invalid_rect);
            invalid_region = std::move(_invalid_region);
            _invalid_rect.clear();
            _invalid_region.clear();
        }

        if (invalid_rect.empty())
            return;

        if (!_renderBuffer || _renderBuffer->size().cx < invalid_rect.right() || _renderBuffer->size().cy < invalid_rect.bottom())
            _renderBuffer = std::make_shared<graphics::Bitmap>(core::si32i{ invalid_rect.right(), invalid_rect.bottom() });

        auto rect = invalid_region.bounds();
        graphics::Graphics graphics(_renderBuffer);
        graphics.setClipRect(rect.to<float32_t>(), false);
        graphics.clear(_color_default);
        for (auto & iter : _controls)
        {
            iter.second->render(graphics, invalid_region);
            if (_exit)
                return;
        }

        if (_exit)
            return;
        //graphics.drawRectangle(rect.to<float32_t>(), graphics::PathStyle().stoke(core::colors::Red).width(2));
        rendered(invalid_region);
        //rendered(core::rc32i{{}, _renderBuffer ->size()});
        static bool save = false;
        if (save)
            _renderBuffer->Save("scene.png");
    }

    void Scene::animationTimerTick(core::timer & t, int64_t tick)
    {
        for (auto & a : _animations)
        {
            if (!(a.second->updateAnimations()))
                a.first |= core::flag::expired;
        }
        _animations.erase(std::remove_if(std::begin(_animations), std::end(_animations), [](const std::pair<core::flags, std::shared_ptr<Control>> & pair) { return pair.first & core::flag::expired; }), _animations.end());

        if (_animations.empty())
            _animation_timer.stop();
    }
}
