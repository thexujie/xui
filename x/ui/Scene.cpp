#include "stdafx.h"
#include "Scene.h"
#include "ui/Control.h"
#include "ui/RadioGroup.h"
#include "Desktop.h"

namespace ui
{
    Scene::Scene(std::shared_ptr<Control> control) : _control(control)
    {
        _ratio = Desktop::instance().ppi().cy / 96.0f;
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

        if (!_th_render.joinable())
            _th_render = std::thread(std::bind(&Scene::renderThread, this));
        _cv_render.notify_all();
    }

    std::shared_ptr<drawing::Bitmap> Scene::readBegin()
    {
        return _draw_buffer;
    }

    void Scene::readEnd()
    {
    }

    void Scene::setEvent(scene_event evt)
    {
        invoke([this, evt]() {evented(evt); });
    }

    std::shared_ptr<RadioGroup> Scene::radioGroup(std::string name)
    {
        auto iter = _radio_groups.find(name);
        if (iter == _radio_groups.end())
        {
            auto group = std::make_shared<RadioGroup>(name);
            _radio_groups[name] = group;
            return group;
        }
        else
        {
            auto ptr = iter->second.lock();
            if (!ptr)
            {
                auto group = std::make_shared<RadioGroup>(name);
                _radio_groups[name] = group;
                return group;
            }
            else
                return ptr;
        }
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
            if (_current_control)
                _current_control->onMouseMove(state);
            break;
        case mouse_action::wheel_v:
            if (auto curr = control()->findChild(state.pos(), nullptr, ui::findchild_flag::accept_wheel_v))
            {
                if (curr->acceptWheelV())
                    curr->onMouseWheel(state);
            }
            else {}
            _updateMouseArea(state, action);
            break;
        case mouse_action::press:
            if (_current_control)
            {
                if (_current_control->captureButtons())
                    captured(true);
                _current_control->onMouseDown(state, button);
            }
            else{}

            if(_current_control != _current_input)
            {
                if (_current_input)
                    _current_input->onBlur();

                if (_current_control && _current_control->acceptInput())
                {
                    _current_input = _current_control;
                    if (_current_input)
                        _current_input->onFocus(_ime_context);
                }
                else
                {
                    _current_input = nullptr;
                    if (_ime_context)
                        _ime_context->setImeMode(ui::ime_mode::disabled);
                }
            }
            break;
        case mouse_action::release:
            if (_current_control)
            {
                _current_control->onMouseUp(state, button);
                if (_current_control->captureButtons())
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
        if (_current_input)
        {
            switch (action)
            {
            case ui::key_action::none:
                break;
            case ui::key_action::press:
                _current_input->onKeyDown(state, key);
                break;
            case ui::key_action::release:
                _current_input->onKeyUp(state, key);
                break;
            default:
                break;
            }
        }
    }

    void Scene::onChar(char32_t ch)
    {
        if (_current_input)
            _current_input->onChar(ch);
    }

    void Scene::_updateMouseArea(const input_state & state, mouse_action action)
    {
        auto ma = action == mouse_action::leave ?  nullptr : control()->findChild(state.pos());
        // do not update while capturing one or more button(s)
        if (_current_control != ma &&
            (!_current_control || !(_current_control->captureButtons() & state.buttons())))
        {
            if (_current_control)
                _current_control->onMouseLeave(state);

            _current_control = ma;

            if (_current_control)
                _current_control->onMouseEnter(state);
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
            if (!_draw_buffer || _draw_buffer->size().cx < invalid_rect.right() || _draw_buffer->size().cy < invalid_rect.bottom())
                _draw_buffer = std::make_shared<drawing::Bitmap>(core::si32i{ invalid_rect.right(), invalid_rect.bottom() });

            drawing::Graphics graphics(_draw_buffer);
            graphics.setClipRect(invalid_rect.to<float32_t>(), true);
            graphics.clear(_color_default);
            auto c = control();
            if (!c)
                continue;
            c->ondraw(graphics, invalid_region);

            static bool save = false;
            if (save)
                _draw_buffer->Save("scene.png");

            //fps.acc(1);
            //auto cost = core::datetime::high_resolution_s() - tms;
            //core::dbg_output(core::string::format(graphics.statistics().total(), " drawcalls, ", cost, " s, fps=", fps.fps()));
            if (_exit)
                break;
            //graphics.drawRectangle(rect.to<float32_t>(), graphics::PathStyle().stoke(core::colors::Red).width(2));

            //invoke([this, region = std::move(invalid_region)]() { rendered(region); });
            rendered(invalid_region);
            //rendered(core::rc32i{{}, _draw_buffer ->size()});

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
