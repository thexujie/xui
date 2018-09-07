#include "stdafx.h"
#include "TextBox.h"
#include <SkTextBlob.h>
#include "ui/renderables/Text.h"
#include "drawing/skia/script.h"

namespace ui::controls
{
    const std::string TEXTBOX_ANIMATION_GROUP_CURSOR = "_textbox.cursor";
    TextBox::TextBox()
    {
        _accept_input = true;
    }

    TextBox::~TextBox()
    {
        
    }

    void TextBox::propertyTableCallback(core::property_table & properties)
    {
        Control::propertyTableCallback(properties);
        properties["text"] = core::make_accessor(&TextBox::setText, &TextBox::text, core::parseString, nullptr);
    }

    void TextBox::propertyTable(core::property_table & properties)
    {
        TextBox::propertyTableCallback(properties);
    }

    void TextBox::setText(const std::string & text)
    {
        std::lock_guard l(*this);
        _textblob = nullptr;
        _text = text;
    }

    core::si32f TextBox::contentSize() const
    {
        _confirmBlob();
        return _textblob ? _textblob->size() : core::si32f();
    }

    void TextBox::updateContent()
    {
        _confirmBlob();
        if (_textblob)
        {
            if(!_text_obj)
            {
                _text_obj = std::make_shared<renderables::Text>(control_ref());
                insert(LOCAL_DEPTH_CONTENT, _text_obj);
            }
            _text_obj->setTextBlob(_textblob);
            _text_obj->setRect(contentBox());
        }

        if (!_cursor_obj)
        {
            _cursor_obj = std::make_shared<renderables::Line>(control_ref());
            _cursor_obj->setVisible(false);
            insert(LOCAL_DEPTH_CONTENT + 1, _cursor_obj);
        }

        drawing::fontmetrics fm(font());
        auto cbox = contentBox();
        _cursor_obj->setRect({ cbox.x, cbox.y, 1, fm.height});
        _cursor_obj->setPoints({ cbox.x, cbox.y }, { cbox.x, cbox.y + fm.height });
        _cursor_obj->setPathStyle(drawing::PathStyle().stoke(core::colors::Red , drawing::stroke_style::solid).width(1));
    }


    std::string TextBox::styleName() const
    {
        if (_pressed)
            return "textbox:active";
        else if (_mousein)
            return "textbox:hover";
        else
            return "textbox";
    }

    void TextBox::onMouseEnter(const mosue_state & state)
    {
        Control::onMouseEnter(state);
        restyle();
    }

    void TextBox::onMouseMove(const mosue_state & state)
    {
        Control::onMouseMove(state);
    }

    void TextBox::onMouseLeave(const mosue_state & state)
    {
        Control::onMouseLeave(state);
        restyle();
    }

    
    void TextBox::onMouseDown(const mosue_state & state)
    {
        Control::onMouseDown(state);
        restyle();
    }

    void TextBox::onMouseUp(const mosue_state & state)
    {
        Control::onMouseUp(state);
        restyle();
    }

    void TextBox::onFocus(std::shared_ptr<ImeContext> imecontext)
    {
        _imecontext = imecontext;
        if(!_cursor_anim)
        {
            auto accessor = make_accessor(&renderables::Line::setVisible, &renderables::Line::visible, core::parseBool, nullptr);
            auto interpolator = std::make_shared<core::property_interpolator_keyframes<bool>>();
            interpolator->set_values({ { 0.0f, true }, { 0.5f, false } });
            {
                std::lock_guard lock(*this);
                if (!_cursor_obj)
                {
                    _cursor_obj = std::make_shared<renderables::Line>(control_ref());
                    _cursor_obj->setVisible(false);
                    insert(LOCAL_DEPTH_CONTENT + 1, _cursor_obj);
                }
            }
            _cursor_anim = std::make_shared<core::property_animation>(_cursor_obj, accessor, interpolator);
            _cursor_anim->setDuration(1s);
            _cursor_anim->setLoop(9999);
            appendAnimation(TEXTBOX_ANIMATION_GROUP_CURSOR, _cursor_anim);
        }
        _cursor_anim->start();
        restyle();
        _updateIme();
    }

    void TextBox::onBlur()
    {
        _imecontext = nullptr;
        //std::lock_guard lock(*this);
        if (_cursor_obj)
            _cursor_obj->setVisible(false);
        if (_cursor_anim)
            _cursor_anim->stop();
        restyle();
    }

    void TextBox::onChar(char32_t ch)
    {
        if (ch <= 0xffff && std::iswcntrl((wint_t)ch))
        {
            if(ch == 8)
            {
                if(!_text.empty())
                {
                    _textblob.reset();
                    refresh();
                    _text.resize(_text.length() - 1);
                }
            }
            return;
        }
        
        char chars[4] = { 0 };
        size_t len = core::utf32_to_utf8(ch, chars);
        _text.append(chars, len);
        _doshaper();

        _shaper->build
        refresh();
    }

    void TextBox::_updateIme()
    {
        if (!_imecontext)
            return;

        _imecontext->setImeMode(_ime_mode);
        if(_ime_mode != ime_mode::disabled)
        {
            _imecontext->setCompositionPos(contentBox().leftTop());
            _imecontext->setCompositionFont(font());
        }
    }

    void TextBox::_confirmBlob() const
    {

    }

    void TextBox::_doshaper()
    {
        if (!_textblob)
            _textblob = std::make_shared<drawing::TextBlob>();

        if (!_shaper)
            _shaper = std::make_shared<drawing::script::Shaper>();

        _shaper->reset(_text);
        _shaper->itermize();
        _shaper->wrap(std::numeric_limits<float32_t>::max(), drawing::wrap_mode::word);

        SkTextBlobBuilder builder;
        _shaper->build(builder, 0);
        auto native = std::shared_ptr<SkTextBlob>(builder.make().release(), [](SkTextBlob * ptr) { if (ptr) SkSafeUnref(ptr); });
        _textblob->setNative(native, _shaper->lineSize(0));
    }
}
