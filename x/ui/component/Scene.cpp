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
    }

    void Scene::invalid(const core::rc32f & rect)
    {
        {
            std::lock_guard<std::mutex> lock(_mtx);
            auto rc = rect.ceil<int32_t>();
            _invalid_rect.unite(rc);
            _invalid_region.addRect(rc);
        }

        //if(!_renderer.valid() || _renderer.wait_for(0s) == std::future_status::ready)
        //    _renderer = std::async(std::bind(&Scene::renderThread, this));

        if (!_th_render.joinable())
            _th_render = std::thread(std::bind(&Scene::renderThread, this));
        _cv_render.notify_all();
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
                    obj = control()->findMouseArea(state.pos(), obj);
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
        auto ma = state.action() == mouse_action::leaving ?  nullptr : control()->findMouseArea(state.pos());
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
        //static core::counter_fps<float64_t, 3> fps(1s);
        while(!_exit)
        {
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
                break;

            auto tms = core::datetime::high_resolution_s();
            if (!_renderBuffer || _renderBuffer->size().cx < invalid_rect.right() || _renderBuffer->size().cy < invalid_rect.bottom())
                _renderBuffer = std::make_shared<graphics::Bitmap>(core::si32i{ invalid_rect.right(), invalid_rect.bottom() });

            auto rect = invalid_region.bounds();
            graphics::Graphics graphics(_renderBuffer);
            graphics.setClipRect(rect.to<float32_t>(), false);
            graphics.clear(_color_default);
            control()->render(graphics, invalid_region);

            //fps.acc(1);
            //auto cost = core::datetime::high_resolution_s() - tms;
            //core::dbg_output(core::string::format(graphics.statistics().total(), " drawcalls, ", cost, " s, fps=", fps.fps()));
            if (_exit)
                break;
            //graphics.drawRectangle(rect.to<float32_t>(), graphics::PathStyle().stoke(core::colors::Red).width(2));
            rendered(invalid_region);
            //rendered(core::rc32i{{}, _renderBuffer ->size()});
            static bool save = false;
            if (save)
                _renderBuffer->Save("scene.png");

            std::unique_lock<std::mutex> lock(_mtx);
            _cv_render.wait(lock, [this]() {return !_invalid_rect.empty() || _exit; });
        }
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
