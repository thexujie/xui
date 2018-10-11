#pragma once
#include "component/Window.h"
#include "Container.h"

namespace ui
{
    enum class form_style
    {
        normal = 0,
        frameless = 0x0001,

        nomin = 0x100000,
        nomax = 0x200000,

        mask_type = 0x000f,
    };

    template<> struct enable_bitmasks<form_style> { static const bool enable = true; };
    typedef core::bitflag<form_style> form_styles;

    class Form : public Container
    {
    public:
        Form(form_styles styles = nullptr);
        Form(core::vec2<core::dimensionf> & size, form_styles styles = nullptr);
        ~Form();

        void setStyles(form_styles styles) { if (styles != _styles) { _styles = styles; stylesChanged(_styles); } }
        form_styles styles() const { return _styles; }
        const core::pt32f & windowPos() const { return _rect_window.pos; }
        //const core::pt32f & windowSize() const { return _rect_window.size; }
        //const core::rc32f & windowRect() const { return _rect_window; }
        void setWindowPos(const core::pt32f & pos);
        void setWindowPos(const core::vec2<core::dimensionf> & pos) { setWindowPos(calc(pos)); }
        void setWindowSize(const core::si32f & size);
        void setWindowSize(const core::vec2<core::dimensionf> & size) { setWindowSize(calc(size)); }

        std::shared_ptr<Scene> formScene() const;

        void show();
        bool shown() const { return _shown; }
        void centerScreen(int32_t screenIndex = 0);

        void enteringScene(std::shared_ptr<Scene> & scene) override;
        void onSizeChanged(const core::si32f & from, const core::si32f & to) override;

    public:
        void onClose();

    public:
        core::event<void(form_styles styles)> stylesChanged;
        core::event<void(const core::pt32f & from, const core::pt32f & to)> windowPosChanged;
        //core::event<void(const core::si32f & from, const core::si32f & to)> windowSizeChanged;
        //core::event<void(const core::rc32f & from, const core::rc32f & to)> windowRectChanged;
        core::event<void(bool shown)> shownChanged;
        core::event<void()> closed;

    private:
        std::shared_ptr<Scene> _form_scene;
        std::shared_ptr<component::Window> _window;
        core::rc32f _rect_window;
        bool _shown = false;
        form_styles _styles = nullptr;
    };
}
