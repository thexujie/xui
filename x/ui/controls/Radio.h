#pragma once
#include "ui/Control.h"

namespace ui::controls
{
    enum class check_state
    {
        unchecked = 0,
        checked,
    };

    class Radio : public Control
    {
    public:
        Radio();
        Radio(std::string text);
        ~Radio();

        static void propertyTableCallback(core::property_table & properties);
        void propertyTable(core::property_table & properties) override;

        void setText(const std::string & text);
        const std::string & text() const { return _text; }

        void setGroup(std::string group);

        core::si32f contentSize() const override;
        std::string styleName() const override;

        void draw(drawing::Graphics & graphics, const drawing::Region & region) const override;

    public:
        void enteringScene(std::shared_ptr<Scene> & scene) override;
        void leavingScene() override;

        void onMouseEnter(const input_state & state) override;
        void onMouseMove(const input_state & state) override;
        void onMouseLeave(const input_state & state) override;
        void onMouseDown(const input_state & state, ui::mouse_button RadioButton) override;
        void onMouseUp(const input_state & state, ui::mouse_button RadioButton) override;

    public:
        void setCheckState(check_state state);

    private:
        void _confirmBlob() const;

        void _setHoleColor(core::color32 color);
        void _setHoleBorderSize(core::dimensionf border_size);
        void _setHoleBorderColor(core::color32 color);
        void _setDotColor();
        void _setDotBorderColor();

    private:
        std::string _text;
        std::string _group;
        check_state _state = check_state::unchecked;

        std::shared_ptr<RadioGroup> _radio_group;

        mutable std::shared_ptr<drawing::TextBlob> _textBlob;

        core::dimensionf _content_spacing = 0.2_em;

        core::dimensionf _hole_border_size = 1_px;
        core::color32 _hole_border_color = core::colors::Gray;
        core::color32 _hole_color = core::colors::Transparent;

        core::dimensionf _dot_border_size = 1_px;
        core::color32 _dot_border_color = core::colors::Gray;
        core::color32 _dot_color = core::colors::Black;
    };

    class RadioGroup
    {
    public:
        RadioGroup(std::string name) : _name(name) {}

        void check(std::shared_ptr<Radio> radio)
        {
            for (auto & r : _radios)
            {
                if (r != radio)
                    r->setCheckState(check_state::unchecked);
            }
        }

        void addRadio(std::shared_ptr<Radio> radio)
        {
            auto iter = std::find(_radios.begin(), _radios.end(), radio);
            if(iter != _radios.end())
                return;

            _radios.push_back(radio);
        }

        void removeRadio(std::shared_ptr<Radio> radio)
        {
            auto iter = std::find(_radios.begin(), _radios.end(), radio);
            if (iter == _radios.end())
                return;

            _radios.erase(iter);
        }

        const std::vector< std::shared_ptr<Radio>> & radios() const { return _radios; }

    private:
        std::string _name;
        std::vector<std::shared_ptr<Radio>> _radios;
    };
}
