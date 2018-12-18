#include "stdafx.h"
#include "Form.h"
#include "win32/Window.h"
#include "Desktop.h"
#include "core/app.h"
#include "RadioGroup.h"
#include "../../xui/main/Menu.h"

namespace ui
{
    Form::Form(form_styles styles, std::shared_ptr<Form> parentForm) :_styles(styles), _parentForm(parentForm)
    {
        _background_color = core::colors::AliceBlue;
        _mouse_through = false;
        _color = core::colors::Black;
    }

    Form::Form(core::vec2<core::dimenf> & size, form_styles styles, std::shared_ptr<Form> parentForm) : _styles(styles), _parentForm(parentForm)
    {
        _background_color = core::colors::AliceBlue;
        _size = size;
        _mouse_through = false;
        _color = core::colors::Black;
    }

    Form::~Form()
    {

    }


    void Form::setParentForm(std::shared_ptr<Form> parentForm)
    {
        // TODO...
        _parentForm = parentForm;
    }

    void Form::setTitle(const std::string & text)
    {
        if(_title != text)
        {
            _title = text;
            titleChanged(_title);
        }
    }

    std::shared_ptr<ui::IWindow> Form::window() const
    {
        if (!_window)
        {
            auto window = std::make_shared<win32::Window>();
            window->attatch(share_ref<Form>());
            const_cast<Form *>(this)->_window = window;
        }
        return _window;
    }

    void Form::setWindowPos(const core::pointf & pos)
    {
        auto pos_old = _window_pos;
        if (pos_old != pos)
        {
            _window_pos = pos;
            if (_window)
                _window->move(pos);
            windowPosChanged(pos_old, pos);
            //windowRectChanged(core::rectf(pos_old, _rect.size), core::rectf(pos, _rect.size));
        }
    }

    void Form::setWindowSize(const core::sizef & size)
    {
        setShowSize(size);
    }

	void Form::setFormState(form_state fs)
	{
		if(_form_state != fs)
		{
			auto old = _form_state;
			_form_state = fs;
            stateChanged(old, _form_state);
			onFormStateChanged(old, _form_state);
		}
	}

    void Form::show(form_state state)
    {
        if (state == form_state::hide)
        {
            if (_menu)
                _menu->hide();
            if (_window)
                _window->show(state);
        }
        else if (state == form_state::minimize)
        {
            if (_menu)
                _menu->hide();
            window()->show(state);
        }
        else
        {
            auto & si = size();
            auto ps = prefferSize();
            core::sizef size = calc(si);
            if (!si.cx.avi())
                size.cx = ps.cx;
            if (!si.cx.avi())
                size.cy = ps.cy;
            place(core::rectf(core::pointf(), { size.cx, size.cy }), size);
            window()->show(state);
        }
    }

    void Form::close()
    {
        invoke([this]() {closed(); });
    }

    void Form::centerScreen(int32_t screenIndex)
    {
        auto rc = Desktop::instance().screen(screenIndex)->rect().to<float32_t>();
        auto s = rect().size;
        auto p = rc.leftTop() + (rc.size - s) * 0.5;
        if (_window)
             _window->move(p);
        else
            setWindowPos(core::pointf{ p.x, p.y });
    }

    void Form::notifyWindowMouse(const input_state & state, mouse_button button, mouse_action action)
    {
        switch (action)
        {
        case mouse_action::enter:
			_setCurrentControl(state, findChild(state.pos()));
            break;
        case mouse_action::leave:
			_setActivedControl(state, nullptr);
			_setCurrentControl(state, nullptr);
            break;
        case mouse_action::move:
			_setCurrentControl(state, _findChild(state.pos()));
			if(_current_control != _current_active)
				_setActivedControl(state, nullptr);

            if (_current_control)
                _current_control->onHover(state);
            break;
        case mouse_action::wheel_v:
            if (auto curr = _findChild(state.pos()))
            {
                while (curr)
                {
                    if (curr->acceptWheelV() && curr->wheelFreedom().any())
                    {
                        curr->onWheel(state);
                        break;
                    }
                    else
                        curr = curr->parent();
                }
            }
			_setCurrentControl(state, _findChild(state.pos()));
            break;
        case mouse_action::press:
            if (_menu)
                _menu->hide();

			_setSelectedControl(state, _current_control);

			if(_current_control && _current_control->captureButtons())
				_setCapturedControl(state, _current_control);
			if(_current_select)
			{
				if(_current_select->acceptInput())
					_setFocusedControl(state, _current_select);
				else if(_current_focus && !_current_focus->holdFocus())
					_setFocusedControl(state, nullptr);
				else {}
			}

			if (button == ui::mouse_button::left)
				_setActivedControl(state, _current_control);
            break;
        case mouse_action::click:
            if (_current_control)
                _current_control->onActive(state);
			_setCurrentControl(state, _findChild(state.pos()));
            break;
        case mouse_action::release:
            if (_current_select)
            {
                if(button == ui::mouse_button::right)
                {
					if (!_menu)
					{
						_menu = std::make_shared<ui::Menu>(ui::form_style::frameless, form());
						_menu->setStyleSheet(form()->styleSheet());
						_menu->setSize(core::vec2<core::dimenf>(core::auto_value, core::auto_value));
						_menu->setBorder({ 1_px, 1_px });
						_menu->setTitle(u8"Menu");
						_menu->setBorderColors({ core::colors::Gray, core::colors::Gray });
						//_menu->setResizeBorders({ 4_px, 4_px });
						//_menu->setLayoutDirection(core::align::top);

						//// ±ÍÃ‚¿∏
						//{
						//    auto title = std::make_shared<ui::controlsex::TitleBar>(form2);
						//    title->setSize({ 100_per, core::auto_value });
						//    title->setBackgroundColor(0xfff1f1f0);
						//    form2->addControl(title);
						//}
						_menu->active += [this](auto action) {if (_current_select) _current_select->onAction(action); };
					}

					_menu->clear();
					_current_select->onPopupMenu(state, *_menu);
					if(_menu->itemCount())
					{
						_menu->show(form_state::noactive);
						_menu->setWindowPos(_window_pos + state.pos());
					}
                }
            }

			if (button == ui::mouse_button::left)
				_setActivedControl(state, nullptr);

			if (!state.buttons().any())
			{
				_setCapturedControl(state, nullptr);
				_setCurrentControl(state, _findChild(state.pos()));
			}
            break;
        default:
			_setCurrentControl(state, _findChild(state.pos()));
            break;
        }
    }

    void Form::notifyWindowKey(const input_state & state, keycode key, key_action action)
    {
        if (_current_focus)
        {
            switch (action)
            {
            case ui::key_action::none:
                break;
            case ui::key_action::press:
                _current_focus->onKeyDown(state, key);
                break;
            case ui::key_action::release:
                _current_focus->onKeyUp(state, key);
                break;
            default:
                break;
            }
        }
    }

    void Form::notifyWindowCharInput(char32_t ch)
    {
        if (_current_focus)
            _current_focus->onChar(ch);
    }

    void Form::notifyWindowCaptured(bool c)
    {
        if (!c)
        {
            if (_captured_control)
                _captured_control = nullptr;
        }
    }

	void Form::notifyWindowFocused(const input_state & state, bool f)
    {
		if (!f)
			_setFocusedControl(state, nullptr);
		else if(_current_select && _current_select->acceptInput())
			_setFocusedControl(state, _current_select);
		else
			_setFocusedControl(state, nullptr);
    }

    void Form::notifyWindowShown(bool s)
    {
        notifyShown(s);
    }

    float_t Form::ratio() const
    {
        return Desktop::instance().scale();
    }

    void Form::setEvent(scene_event evt)
    {
        invoke([this, evt]() {evented(evt); });
    }

    void Form::invalidate(const core::rectf & rect)
    {

        auto rc = rect.intersected(core::rectf(core::pointf(), _rect.size)).ceil<int32_t>();
        _invalid_region.addRect(rc);
        if (!_invaliding && validCompleted())
        {
            _invaliding = true;
            invoke([this]() { _invaliding = false; _render(); });
        }
    }

    void Form::animate()
    {
        if (!_animation_timer.running())
        {
            _animation_timer.tick += std::bind(&Form::_animationTimerTick, this, std::placeholders::_1, std::placeholders::_2);
            _animation_timer.start(1000ms / 60);
        }
    }

    std::shared_ptr<Control> Form::findChild(const core::pointf & pos, std::shared_ptr<Control> last, findchild_flags flags) const
    {
        if (_clip_clild && !_rect.contains(pos))
            return nullptr;

        if (_resize_borders)
        {
            auto b = calc(_resize_borders.value);
            if (!_rect.expanded(-b).contains(pos))
                return control_ref();
        }

        auto child = Container::findChild(pos, last, flags);
        return child ? child : control_ref();
    }

    void Form::onSizeChanged(const core::sizef & from, const core::sizef & to)
    {
        if (_window)
            _window->resize(to);
        Container::onSizeChanged(from, to);
    }

    void Form::onPaint(drawing::Graphics & graphics, const core::rectf & clip) const
    {
        Container::onPaint(graphics, clip);
    }

    hittest_form Form::hitTestForm(const core::pointf & pos) const
    {
        if(!_resize_borders)
            return Container::hitTestForm(pos);

        enum
        {
            left = 0x0001,
            right = 0x0002,
            top = 0x0004,
            bottom = 0x0008,
        };

		if(_form_state == form_state::maximize)
			return Container::hitTestForm(pos);

        auto b = calc(_resize_borders.value);
        uint32_t temp = 0;
        if (pos.x < b.bleft)
            temp |= left;
        else if (pos.x >= _rect.cx - b.bright)
            temp |= right;
        else {}
        if (pos.y < b.btop)
            temp |= top;
        else if (pos.y >= _rect.cy - b.bbottom)
            temp |= bottom;
        else {}

        switch (temp)
        {
        case left | top: return hittest_form::resize_leftTop;
        case top: return hittest_form::resize_top;
        case right | top: return hittest_form::resize_rightTop;
        case right: return hittest_form::resize_right;
        case right | bottom: return hittest_form::resize_rightBottom;
        case bottom: return hittest_form::resize_bottom;
        case left | bottom: return hittest_form::resize_leftBottom;
        case left: return hittest_form::resize_left;
        default: return Container::hitTestForm(pos);
        }
    }

	void Form::onFormStylesChanged(form_styles from, form_styles to)
    {
		stylesChanged(from, to);
    }

	void Form::onFormStateChanged(form_state from, form_state to)
    {
    }

	void Form::onAction(action_t action)
	{
        core::event_flags flags;
		activing(action, flags);
        if (flags.any(core::event_flag::rejected))
            return;

		switch(action)
		{
		case system_action::none: 
			break;
		case system_action::close:
            show(form_state::hide);
            invoke([this]() { closed(); });
			break;
		case system_action::minimize:
			show(form_state::minimize);
			break;
		case system_action::maximize:
			show(_form_state == form_state::maximize ? form_state::normalize : form_state::maximize);
			break;
		default:;
		}
	}

    std::shared_ptr<RadioGroup> Form::radioGroup(std::string name)
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

    void Form::_updateMouseArea(const input_state & state, mouse_action action)
    {
        if (_captured_control)
            return;

        auto ma = action == mouse_action::leave || !state.hoving() ? nullptr : findChild(state.pos());
        // do not update while capturing one or more button(s)
        if (_current_control != ma &&
            (!_current_control || !(_current_control->captureButtons() & state.buttons())))
        {
			_setCurrentControl(state, ma);
        }
    }

    void Form::_render()
    {
        //static core::counter_fps<float64_t, 3> fps(1s);
        if (_invalid_region.empty())
            return;

        auto tms = core::datetime::high_resolution_s();
        if (!_draw_buffer)
            _draw_buffer = std::make_shared<drawing::Surface>();

        auto bounds = _invalid_region.bounds();
        _invalid_region.clear();

        if (_draw_buffer->size().cx < bounds.right() || _draw_buffer->size().cy < bounds.bottom())
            _draw_buffer->resize(core::sizei{ bounds.right(), bounds.bottom() });

        auto boundsf = bounds.to<float32_t>();
        drawing::Graphics graphics(_draw_buffer);
        graphics.save();
        graphics.setClipRect(bounds.to<float32_t>(), true);
        graphics.clear(0);
        onPaint(graphics, boundsf);
        graphics.restore();
        static bool save = false;
        if (save)
            _draw_buffer->Save("scene.png");

        //fps.acc(1);
        //auto cost = core::datetime::high_resolution_s() - tms;
        //core::dbg_output(core::string::format(graphics.statistics().total(), " drawcalls, ", cost, " s, fps=", fps.fps()));
        //graphics.drawRectangle(rect.to<float32_t>(), graphics::PathStyle().stoke(core::colors::Red).width(2));

        //invoke([this, region = std::move(invalid_region)]() { rendered(region); });
        rendered(bounds);
        //rendered(core::recti{{}, _draw_buffer ->size()});
    }

    void Form::_animationTimerTick(core::timer &, int64_t)
    {
        auto num = onAnimate();
        if (!num)
            _animation_timer.stop();
    }

	std::shared_ptr<Control> Form::_findChild(const input_state & state, std::shared_ptr<Control> last, findchild_flags flags)
    {
		if (_captured_control)
			return _captured_control;
		return findChild(state.pos(), last, flags);
    }

	void Form::_setCurrentControl(const input_state & state, std::shared_ptr<Control> control)
    {
		if (_current_control != control)
		{
			if (_current_control)
				_current_control->notifyHovered(state, false);

			_current_control = control;

			if (_current_control)
				_current_control->notifyHovered(state, true);
		}
    }

	void Form::_setSelectedControl(const input_state & state, std::shared_ptr<Control> control)
    {
		if (_current_select != control)
		{
			if (_current_select)
				_current_select->notifySelected(state, false);

			_current_select = control;

			if (_current_select)
				_current_select->notifySelected(state, true);
		}
    }

	void Form::_setActivedControl(const input_state & state, std::shared_ptr<Control> control)
    {
		if (_current_active != control)
		{
			if (_current_active)
				_current_active->notifyActived(state, false);

			_current_active = control;

			if (_current_active)
				_current_active->notifyActived(state, true);
		}
    }

	void Form::_setFocusedControl(const input_state & state, std::shared_ptr<Control> control)
    {
		if (_current_focus != control)
		{
			if (_current_focus)
				_current_focus->notifyFocused(state, false);

			_current_focus = control;

			if (_current_focus)
				_current_focus->notifyFocused(state, true);

			if(!_current_focus)
			{
				if (auto ic = imeContext())
					ic->setImeMode(ui::ime_mode::disabled);
			}
		}
    }

	void Form::_setCapturedControl(const input_state & state, std::shared_ptr<Control> control)
    {
		captured(!!control);
		_captured_control = control;
    }
}
