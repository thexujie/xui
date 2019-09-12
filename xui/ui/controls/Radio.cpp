#include "stdafx.h"
#include "Radio.h"

namespace ui::controls
{
    Radio::Radio()
    {
    }

	Radio::Radio(const std::u8string & text): _text(text)
	{
	}

    Radio::~Radio()
    {
        
    }

    void Radio::propertyTableCallback(core::property_table & properties)
    {
        Control::propertyTableCallback(properties);
        properties[u8"text"] = core::make_accessor(&Radio::setText, &Radio::text);
        properties[u8"hole-color"] = core::make_accessor(&Radio::_setHoleColor, &Radio::_hole_color);
        properties[u8"hole-border-size"] = core::make_accessor(&Radio::_setHoleBorderSize, &Radio::_hole_border_size);
        properties[u8"hole-border-color"] = core::make_accessor(&Radio::_setHoleBorderColor, &Radio::_hole_border_color);
    }

    void Radio::propertyTable(core::property_table & properties)
    {
        Radio::propertyTableCallback(properties);
    }

    void Radio::setText(const std::u8string & text)
    {
		_text = text;
        refresh();
    }

    std::u8string Radio::styleName() const
    {
        if (_actived)
            return u8"radio:active";
        else if (_hovered)
            return u8"radio:hover";
        else
            return u8"radio";
    }

	void Radio::onEnterScene()
	{
		_text_object = scene()->createText();
		ui::base::Radio::onEnterScene();
	}

	void Radio::onLeaveScene()
	{
		_text_object.reset();
		ui::base::Radio::onLeaveScene();
	}
	
    void Radio::update()
    {
		_text_object->update(_text, font(), color());
        core::sizef size = _text_object->bounds();
        size.cx += scene()->graphicsService().font_metrics(font()).height + calc(_content_spacing);
        setContentSize(size);
    }

    void Radio::paint(drawing::Graphics & graphics, const core::rectf & clip) const
    {
        auto box = paddingBox();
        auto fm = scene()->graphicsService().font_metrics(font());

        core::rectf rc_hole(box.leftTop(), { fm.height, fm.height });
        rc_hole.expand(-calc(_hole_border_size) * 0.5f);
        if (_hole_color.visible())
            graphics.drawEllipse(rc_hole, drawing::PathFormat()._fill(_hole_color));
        if (_hole_border_color.visible())
            graphics.drawEllipse(rc_hole, drawing::PathFormat()._stoke(_hole_border_color, calc(_hole_border_size)));

        if(_state == check_state::checked)
        {
            core::rectf rc_dot = rc_hole.expanded(-fm.height * 0.2f);
            if (_dot_color.visible())
                graphics.drawEllipse(rc_dot, drawing::PathFormat()._fill(_dot_color));
            if (_dot_border_color.visible())
                graphics.drawEllipse(rc_dot, drawing::PathFormat()._stoke(_dot_border_color, calc(_dot_border_size)));
        }

        graphics.drawText(*_text_object, contentBox().leftTop().offset(fm.height + calc(_content_spacing), 0), drawing::StringFormat()._color(color()));
    }

    void Radio::onActiveOut(const input_state & state)
    {
        Control::onActiveOut(state);
        restyle();
        if(_state != check_state::checked)
            setCheckState(check_state::checked);
    }

    void Radio::_setHoleColor(core::color color)
    {
        if(_hole_color != color)
        {
            _hole_color = color;
            repaint();
        }
    }

    void Radio::_setHoleBorderSize(core::dimenf border_size)
    {
        if (_hole_border_size != border_size)
        {
            _hole_border_size = border_size;
            repaint();
        }
    }

    void Radio::_setHoleBorderColor(core::color color)
    {
        if (_hole_border_color != color)
        {
            _hole_border_color = color;
            repaint();
        }
    }
}
