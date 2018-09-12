#include "stdafx.h"
#include "Scene.h"
#include "ui/Control.h"

namespace ui
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

    core::error Scene::animate()
    {
        if(!_animation_timer.running())
        {
            _animation_timer.tick += std::bind(&Scene::animationTimerTick, this, std::placeholders::_1, std::placeholders::_2);
            _animation_timer.start(1000ms / 60);
        }
        return core::error_ok;
    }

    void Scene::onMouse(const input_state & state, mouse_button button, mouse_action action)
    {
        switch (action)
        {
        case mouse_action::enter:
            _updateMouseArea(state, action);
            break;
        case mouse_action::move:
            _updateMouseArea(state, action);
            if (_interactbale_current)
                _interactbale_current->onMouseMove(state);
            break;
        case mouse_action::wheel_v:
            if (_interactbale_current && _interactbale_current->acceptWheelV())
                _interactbale_current->onMouseWheel(state);
            else
            {
                auto obj = _interactbale_current;
                while (true)
                {
                    obj = control()->findChild(state.pos(), obj);
                    if (!obj)
                        break;

                    if (!obj->acceptWheelV())
                        continue;

                    obj->onMouseWheel(state);
                    break;
                }
            }
            _updateMouseArea(state, action);
            break;
        case mouse_action::press:
            if (_interactbale_current)
            {
                if (_interactbale_current->captureButtons())
                    captured(true);
                _interactbale_current->onMouseDown(state, button);
            }
            else{}

            if(_interactbale_current != _interactbale_input)
            {
                if (_interactbale_input)
                    _interactbale_input->onBlur();

                if (_interactbale_current && _interactbale_current->acceptInput())
                {
                    _interactbale_input = _interactbale_current;
                    if (_interactbale_input)
                        _interactbale_input->onFocus(_imecontext);
                }
                else
                {
                    _interactbale_input = nullptr;
                    if (_imecontext)
                        _imecontext->setImeMode(ui::ime_mode::disabled);
                }
            }
            break;
        case mouse_action::release:
            if (_interactbale_current)
            {
                _interactbale_current->onMouseUp(state, button);
                if (_interactbale_current->captureButtons())
                    captured(false);
            }
            _updateMouseArea(state, action);
            break;
        default:
            _updateMouseArea(state, action);
            break;
        }
    }

    void Scene::onKey(const input_state & state, keycode key, key_action action)
    {
        if (_interactbale_input)
        {
            switch (action)
            {
            case ui::key_action::none:
                break;
            case ui::key_action::press:
                _interactbale_input->onKeyDown(state, key);
                break;
            case ui::key_action::release:
                _interactbale_input->onKeyUp(state, key);
                break;
            default:
                break;
            }
        }
    }

    void Scene::onChar(char32_t ch)
    {
        if (_interactbale_input)
            _interactbale_input->onChar(ch);
    }

    void Scene::_updateMouseArea(const input_state & state, mouse_action action)
    {
        auto ma = action == mouse_action::leave ?  nullptr : control()->findChild(state.pos());
        // do not update while capturing one or more button(s)
        if (_interactbale_current != ma &&
            (!_interactbale_current || !(_interactbale_current->captureButtons() & state.buttons())))
        {
            if (_interactbale_current)
                _interactbale_current->onMouseLeave(state);

            _interactbale_current = ma;

            if (_interactbale_current)
                _interactbale_current->onMouseEnter(state);
        }
    }

    void Scene::renderThread()
    {
        //static core::counter_fps<float64_t, 3> fps(1s);
        while(!_exit)
        {
            core::rc32i invalid_rect;
            drawing::Region invalid_region;
            {
                std::lock_guard<std::mutex> lock(_mtx);
                invalid_rect = std::move(_invalid_rect);
                invalid_region = std::move(_invalid_region);
                _invalid_rect.clear();
                _invalid_region.clear();
            }

            if (invalid_rect.empty())
                continue;

            auto tms = core::datetime::high_resolution_s();
            if (!_renderBuffer || _renderBuffer->size().cx < invalid_rect.right() || _renderBuffer->size().cy < invalid_rect.bottom())
                _renderBuffer = std::make_shared<drawing::Bitmap>(core::si32i{ invalid_rect.right(), invalid_rect.bottom() });

            drawing::Graphics graphics(_renderBuffer);
            graphics.setClipRect(invalid_rect.to<float32_t>(), true);
            graphics.clear(_color_default);
            auto c = control();
            if (!c)
                continue;
            c->render(graphics, invalid_region);

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

    void Scene::animationTimerTick(core::timer &, int64_t )
    {
        auto c = control();
        if (!c)
            return;

        int32_t num = c->animate();
        if (!num)
            _animation_timer.stop();
    }
}
