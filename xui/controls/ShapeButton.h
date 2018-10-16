#pragma once

namespace xui::controls
{
    class ShapeButton : public ui::base::Button
    {
    public:
        ShapeButton();
        ShapeButton(std::string text);
        ~ShapeButton();

        static void propertyTableCallback(core::property_table & properties);
        void propertyTable(core::property_table & properties) override;

        void setPath(std::shared_ptr<drawing::Path> path);
        void setHitTestForm(ui::hittest_form hf) { _hittest_form = hf; }

        std::string styleName() const override;

        void draw(drawing::Graphics & graphics, const drawing::Region & region) const override;

    private:
        std::shared_ptr<drawing::Path> _path;
        ui::hittest_form _hittest_form = ui::hittest_form::client;
    };
}
