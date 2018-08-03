#include "stdafx.h"
#include "Button.h"
#include "renderables/Text.h"
#include "interactables/MouseRectangle.h"

namespace controls
{
    Button::Button()
    {
        _border = core::vec4<core::dimensionf>{ 1_px };
        _padding = {1_em, 0.5_em};
    }

    Button::Button(std::string text) : _text(text)
    {
        _border = core::vec4<core::dimensionf>{ 1_px };
        _padding = { 1_em, 0.5_em };
    }

    Button::~Button()
    {
        
    }

    core::si32f Button::contentSize() const
    {
        _confirmBlob();
        return _textBlob ? _textBlob->size() : core::si32f();
    }

    template<typename T, typename SrcT>
    std::shared_ptr<T> dynamic_to(std::shared_ptr<SrcT> src)
    {
        return std::dynamic_pointer_cast<T>(src);
    }
    //template<class _Rx,
    //    class _Ty>
    //    _NODISCARD inline _Mem_fn<_Rx _Ty::*> mem_fn(_Rx _Ty::* _Pm) _NOEXCEPT
    //{	// wrap a pointer to member function/data
    //    return (_Mem_fn<_Rx _Ty::*>(_Pm));
    //}

    void Button::updateContent(std::shared_ptr<component::View> & view)
    {
        _confirmBlob();
        if (_textBlob)
        {
            if(!_text_obj)
            {
                _text_obj = std::make_shared<renderables::Text>(_textBlob);
                view->insert(_text_obj);
            }
            _text_obj->setPos(contentBox().pos);
        }

        if(!_mosuerectangle)
        {
            _mosuerectangle = std::make_shared<interactables::MouseRectangle>();
            view->insert(_mosuerectangle);

            _mosuerectangle->mouseEnter += std::weak_binder(std::mem_fn(&Button::onMouseEnter), shared_from_this());
            //_mosuerectangle->mouseEnter += std::weak_bind(&Button::onMouseEnter, dynamic_to<Button>(shared_from_this()), std::placeholders::_1);
            _mosuerectangle->mouseMove += std::weak_bind(&Button::onMouseMove, dynamic_to<Button>(shared_from_this()), std::placeholders::_1);
            _mosuerectangle->mouseLeave += std::weak_bind(&Button::onMouseLeave, dynamic_to<Button>(shared_from_this()), std::placeholders::_1);
            _mosuerectangle->mouseDown += std::weak_binder(std::mem_fn(&Button::onMouseDown), shared_from_this());
            _mosuerectangle->mouseUp += std::weak_binder(std::mem_fn(&Button::onMouseUp), shared_from_this());
        }

        _mosuerectangle->setRect(box());
    }

    void Button::onMouseEnter(const component::mosue_state & state)
    {
        setBorder({ 2_px });
        update();
    }

    void Button::onMouseMove(const component::mosue_state & state)
    {
        
    }

    void Button::onMouseLeave(const component::mosue_state & state)
    {
        setBorder({ 1_px });
        update();
    }

    
    void Button::onMouseDown(const component::mosue_state & state)
    {
        setBackgroundColor(0x80808080);
        update();
    }

    void Button::onMouseUp(const component::mosue_state & state)
    {
        setBackgroundColor(core::colors::Auto);
        update();
    }

    void Button::_confirmBlob() const
    {
        if (!_textBlob)
        {
            graphics::StringFormat format(font());
            format.color(color());
            const_cast<std::shared_ptr<graphics::TextBlob> &>(_textBlob) = std::make_shared<graphics::TextBlob>(_text, format);
        }
    }

    void Button::_applyStyle()
    {
        _style->name;
    }
}
