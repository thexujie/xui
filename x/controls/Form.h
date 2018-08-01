#pragma once
#include "Container.h"
#include "component/Window.h"

namespace controls
{
    class Form : public Container
    {
    public:
        Form();
        Form(core::vec2<core::dimensionf> & size);
        ~Form();

        const core::pt32f & windowPos() const { return _rect_window.pos; }
        //const core::pt32f & windowSize() const { return _rect_window.size; }
        //const core::rc32f & windowRect() const { return _rect_window; }
        void setWindowPos(const core::pt32f & pos);
        void setWindowPos(const core::vec2<core::dimensionf> & pos) { setWindowPos(calc(pos)); }
        //void setWindowSize(const core::si32f & size);
        //void setWindowSize(const core::vec2<core::dimensionf> & size) { setWindowSize(calc(size)); }

        void show();
        void centerScreen(int32_t screenIndex = 0);

        virtual void enteringScene(std::shared_ptr<component::Scene> & scene);
        virtual void onSizeChanged(const core::si32f & from, const core::si32f & to);

    public:
        void onClose();

    public:
        core::event<void(const core::pt32f & from, const core::pt32f & to)> windowPosChanged;
        //core::event<void(const core::si32f & from, const core::si32f & to)> windowSizeChanged;
        //core::event<void(const core::rc32f & from, const core::rc32f & to)> windowRectChanged;
        core::event<void(bool shown)> shownChanged;
        core::event<void()> closed;

    private:
        std::shared_ptr<controls::component::Scene> _scene;
        std::shared_ptr<component::Window> _window;
        core::rc32f _rect_window;
        bool _shown = false;
    };
}
