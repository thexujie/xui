#include "stdafx.h"
#include "Form.h"
#include "win32/Window.h"
#include "Desktop.h"
#include "core/app.h"

namespace ui
{
    Form::Form(form_styles styles) :_styles(styles)
    {
        _background_color = core::colors::AliceBlue;
        _mouse_through = false;
        _color = core::colors::Black;
    }

    Form::Form(core::vec2<core::dimenf> & size, form_styles styles) : _styles(styles)
    {
        _background_color = core::colors::AliceBlue;
        _size = size;
        _mouse_through = false;
        _color = core::colors::Black;
    }

    Form::~Form()
    {

    }

    void Form::setTitle(const std::string & text)
    {
        if(_title != text)
        {
            _title = text;
            titleChanged(_title);
        }
    }

    void Form::setWindowPos(const core::pointf & pos)
    {
        auto pos_old = _rect_window.pos;
        if (pos_old != pos)
        {
            _rect_window.pos = pos;
            if (_window)
                _window->move(pos);
            windowPosChanged(pos_old, pos);
            //windowRectChanged(core::rectf(pos_old, _rect_window.size), core::rectf(pos, _rect_window.size));
        }
    }

    void Form::setWindowSize(const core::sizef & size)
    {
        setShowSize(size);
    }

    void Form::setFormScene(std::shared_ptr<Scene> s)
    {
        if(_scene.expired())
        {
            onEnteringScene(s);
            onEnterScene(s);

            auto form_size = calc(_size);
            place(core::rectf(core::pointf(), form_size), form_size);
        }
    }

	void Form::setFormState(form_state fs)
	{
		if(_form_state != fs)
		{
			auto old = _form_state;
			_form_state = fs;
			onFormStateChanged(old, _form_state);
		}
	}

    void Form::show(form_state fs)
    {
        if (!scene())
            throw core::exception(core::error_state);

        if (_form_state != fs)
        {
			auto old = _form_state;
            _form_state = fs;

            if (!_window)
            {
                auto window = std::make_shared<win32::Window>();
                window->attatch(share_ref<Form>());
                _window = window;
            }
			stateChanged(_form_state, fs);
        }
    }

    void Form::close()
    {
        invoke([this]() {closed(); });
    }

    void Form::centerScreen(int32_t screenIndex)
    {
        auto rc = Desktop::instance().screen(screenIndex)->rect().to<float32_t>();
        auto s = calc(size());
        auto p = rc.leftTop() + (rc.size - s) * 0.5;
        setWindowPos(p);
    }

    float_t Form::ratio() const
    {
        if (auto s = scene())
            return s->ratio();
        else
            return 1.0f;
    }

    void Form::invalidate(const core::rectf & rect)
    {
        if(auto s = scene())
          s->invalidate(rect.intersected(core::rectf(core::pointf(), realSize())));
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

    void Form::onEnteringScene(std::shared_ptr<Scene> & scene)
    {
        Container::onEnteringScene(scene);
    }

    void Form::onSizeChanged(const core::sizef & from, const core::sizef & to)
    {
        if (_window)
            _window->resize(to);
        Container::onSizeChanged(from, to);
    }

    void Form::prepaint(drawing::Graphics & graphics, const core::rectf & clip) const
    {
        Container::prepaint(graphics, clip);
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
		stateChanged(from, to);
    }

    void Form::onClose()
    {
        invoke([this]() {closed(); });
    }

	void Form::onTitleAction(title_action action)
	{
		switch(action)
		{
		case title_action::none: 
			break;
		case title_action::close:
			onClose();
			break;
		case title_action::minimize:
			show(form_state::minimize);
			break;
		case title_action::maximize:
			show(_form_state == form_state::maximize ? form_state::normalize : form_state::maximize);
			break;
		default:;
		}
	}
}
