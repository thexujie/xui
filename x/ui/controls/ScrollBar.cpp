#include "stdafx.h"
#include "ScrollBar.h"
#include "ui/renderables/Text.h"
#include "ui/interactables/MouseRectangle.h"

namespace ui::controls
{
    ScrollBar::ScrollBar()
    {
        _border = core::vec4<core::dimensionf>{ 1_px };
        _padding = {1_em, 0.5_em};
    }

    ScrollBar::ScrollBar(std::string text) : _text(text)
    {
        _border = core::vec4<core::dimensionf>{ 1_px };
        _padding = { 1_em, 0.5_em };
    }

    ScrollBar::~ScrollBar()
    {
        
    }

    void ScrollBar::propertyTableCallback(core::property_table & properties)
    {
        Control::propertyTableCallback(properties);
        properties["bar-color"] = make_accessor(&ScrollBar::setBarColor, &ScrollBar::barColor, core::parseColor, nullptr);
        properties["bar-border"] = make_accessor(&ScrollBar::setBarBorder, &ScrollBar::barBorder, core::parseDimension4D, nullptr);
        properties["bar-border-color"] = make_accessor(&ScrollBar::setBarBorderColors, &ScrollBar::barBorderColors, core::parseColor4D, nullptr);
        properties["bar-border-styles"] = make_accessor(&ScrollBar::setBarBorderStyles, &ScrollBar::barBorderStyles, graphics::parseStrokeStyle4D, nullptr);
    }

    void ScrollBar::propertyTable(core::property_table & properties)
    {
        ScrollBar::propertyTableCallback(properties);
    }

    void ScrollBar::setText(const std::string & text)
    {
        _textBlob = nullptr;
        _text = text;
    }

    core::si32f ScrollBar::contentSize() const
    {
        _confirmBlob();
        return _textBlob ? _textBlob->size() : core::si32f();
    }

    void ScrollBar::updateContent(std::shared_ptr<component::View> & view)
    {
        _confirmBlob();

        if(!_mc_bar)
        {
            _mc_bar = std::make_shared<interactables::MouseRectangle>();
            view->insert(_mc_bar);

            _mc_bar->mouseEnter += std::weak_binder(std::mem_fn(&ScrollBar::onMouseEnter), shared_from_this());
            _mc_bar->mouseMove += std::weak_bind(&ScrollBar::onMouseMove, share_ref<ScrollBar>(), std::placeholders::_1);
            _mc_bar->mouseLeave += std::weak_bind(&ScrollBar::onMouseLeave, share_ref<ScrollBar>(), std::placeholders::_1);
            _mc_bar->mouseDown += std::weak_binder(std::mem_fn(&ScrollBar::onMouseDown), shared_from_this());
            _mc_bar->mouseUp += std::weak_binder(std::mem_fn(&ScrollBar::onMouseUp), shared_from_this());
        }

        _mc_bar->setRect(box());
    }


    std::string ScrollBar::styleName() const
    {
        bool mousein = false;
        bool pressed = false;
        if (_mc_bar)
        {
            mousein = _mc_bar->mousein();
            pressed = _mc_bar->pressed();
        }

        if (pressed)
            return "ScrollBar:active";
        else if (mousein)
            return "ScrollBar:hover";
        else
            return "ScrollBar";
    }

    void ScrollBar::onMouseEnter(const component::mosue_state & state)
    {
        updateStyle();
    }

    void ScrollBar::onMouseMove(const component::mosue_state & state)
    {
        
    }

    void ScrollBar::onMouseLeave(const component::mosue_state & state)
    {
        updateStyle();
    }

    
    void ScrollBar::onMouseDown(const component::mosue_state & state)
    {
        updateStyle();
    }

    void ScrollBar::onMouseUp(const component::mosue_state & state)
    {
        updateStyle();
    }

    void ScrollBar::_confirmBlob() const
    {
        if (!_textBlob)
        {
            graphics::StringFormat format(font());
            format.color(color());
            const_cast<std::shared_ptr<graphics::TextBlob> &>(_textBlob) = std::make_shared<graphics::TextBlob>(_text, format);
        }
    }
}
