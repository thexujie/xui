#include "stdafx.h"
#include "Radio.h"

namespace ui::controls
{
    Radio::Radio()
    {
    }

    Radio::Radio(std::string text) : _text(text)
    {
    }

    Radio::~Radio()
    {
        
    }

    void Radio::propertyTableCallback(core::property_table & properties)
    {
        Control::propertyTableCallback(properties);
        properties["text"] = core::make_accessor(&Radio::setText, &Radio::text, core::parseString, nullptr);
        properties["hole-color"] = core::make_accessor(&Radio::_setHoleColor, &Radio::_hole_color, core::parseColor, nullptr);
        properties["hole-border-size"] = core::make_accessor(&Radio::_setHoleBorderSize, &Radio::_hole_border_size, core::parseDimension, nullptr);
        properties["hole-border-color"] = core::make_accessor(&Radio::_setHoleBorderColor, &Radio::_hole_border_color, core::parseColor, nullptr);
    }

    void Radio::propertyTable(core::property_table & properties)
    {
        Radio::propertyTableCallback(properties);
    }

    void Radio::setText(const std::string & text)
    {
        _textBlob = nullptr;
        _text = text;
    }

    void Radio::setGroup(std::string group)
    {
        if(group != _group)
        {
            if(_radio_group)
            {
                _radio_group->removeRadio(share_ref<Radio>());
                _radio_group = nullptr;
            }
            _group = group;
            if(!group.empty())
            {
                if (auto s = scene())
                    _radio_group = s->radioGroup(group);
            }

        }
    }

    core::si32f Radio::contentSize() const
    {
        _confirmBlob();
        core::si32f size = _textBlob ? _textBlob->size() : core::si32f();
        size.cx += drawing::fontmetrics(font()).height + calc(_content_spacing);
        return size;
    }

    std::string Radio::styleName() const
    {
        if (_pressed)
            return "radio:active";
        else if (_mousein)
            return "radio:hover";
        else
            return "radio";
    }

    void Radio::draw(drawing::Graphics & graphics, const drawing::Region & region) const
    {
        auto box = paddingBox();
        auto fm = drawing::fontmetrics(font());
        std::lock_guard l(*this);
        _drawBackground(graphics);

        core::rc32f rc_hole(box.leftTop(), { fm.height, fm.height });
        rc_hole.expand(-calc(_hole_border_size) * 0.5f);
        if (_hole_color.visible())
            graphics.drawEllipse(rc_hole, drawing::PathStyle().fill(_hole_color));
        if (_hole_border_color.visible())
            graphics.drawEllipse(rc_hole, drawing::PathStyle().stoke(_hole_border_color, calc(_hole_border_size)));

        if(_state == check_state::checked)
        {
            core::rc32f rc_dot = rc_hole.expanded(-fm.height * 0.2f);
            if (_dot_color.visible())
                graphics.drawEllipse(rc_dot, drawing::PathStyle().fill(_dot_color));
            if (_dot_border_color.visible())
                graphics.drawEllipse(rc_dot, drawing::PathStyle().stoke(_dot_border_color, calc(_dot_border_size)));
        }

        if (_textBlob)
            graphics.drawTextBlob(*_textBlob, contentBox().leftTop().offset(fm.height + calc(_content_spacing), 0));
        _drawBorder(graphics);
    }

    void Radio::enteringScene(std::shared_ptr<Scene> & scene)
    {
        if (!_group.empty())
        {
            _radio_group = scene->radioGroup(_group);
            _radio_group->addRadio(share_ref<Radio>());
        }
        Control::enteringScene(scene);
    }

    void Radio::leavingScene()
    {
        if (_radio_group)
        {
            _radio_group->removeRadio(share_ref<Radio>());
            _radio_group = nullptr;
        }
        Control::leavingScene();
    }

    void Radio::onMouseEnter(const input_state & state)
    {
        Control::onMouseEnter(state);
        restyle();
    }

    void Radio::onMouseMove(const input_state & state)
    {
        Control::onMouseMove(state);
    }

    void Radio::onMouseLeave(const input_state & state)
    {
        Control::onMouseLeave(state);
        restyle();
    }

    
    void Radio::onMouseDown(const input_state & state, ui::mouse_button RadioButton)
    {
        Control::onMouseDown(state, RadioButton);
        restyle();
    }

    void Radio::onMouseUp(const input_state & state, ui::mouse_button RadioButton)
    {
        Control::onMouseUp(state, RadioButton);
        restyle();
        setCheckState(_state == check_state::checked ? check_state::unchecked : check_state::checked);
    }

    void Radio::setCheckState(check_state state)
    {
        if(_state != state)
        {
            if (_radio_group && state == check_state::checked)
                _radio_group->check(share_ref<Radio>());
            _state = state;
            refresh();
        }
    }

    void Radio::_confirmBlob() const
    {
        if (!_textBlob)
        {
            drawing::StringFormat format(font());
            format.color(color());
            _textBlob = std::make_shared<drawing::TextBlob>(_text, format);
        }
    }

    void Radio::_setHoleColor(core::color32 color)
    {
        if(_hole_color != color)
        {
            _hole_color = color;
            refresh();
        }
    }

    void Radio::_setHoleBorderSize(core::dimensionf border_size)
    {
        if (_hole_border_size != border_size)
        {
            _hole_border_size = border_size;
            refresh();
        }
    }

    void Radio::_setHoleBorderColor(core::color32 color)
    {
        if (_hole_border_color != color)
        {
            _hole_border_color = color;
            refresh();
        }
    }
}
