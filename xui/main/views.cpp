#include "stdafx.h"
#include "ListView.h"
#include "MenuView.h"

struct inputdata
{
    int32_t d1;
    int32_t d2;
    char d3;
};

class method_member
{
public:
    virtual ~method_member() {}
    virtual std::any call(const std::vector<std::any> & params) = 0;
};


template<typename FunT>
class method_member_type
{
public:
    std::any call(const std::vector<std::any> & params)
    {
        
    }


};


class UserViewItem : public ui::ViewItem
{
public:
    UserViewItem(std::string text, drawing::wrap_mode wrap_mode = drawing::wrap_mode::none) : _text(text), _wrap_mode(wrap_mode) {}
    void paint(drawing::Graphics & graphics, const core::rectf & clip)
    {
        
    }

    core::sizef prefferdSize(float32_t width) const
    {
        auto v = view();
        if (!v)
            return {};

        drawing::TextWraper wraper(_text);
        wraper.itermize(v->font(), core::colors::Auto);
        wraper.layout(width, _wrap_mode);
        auto bounds = wraper.bounds();
        return { bounds.cx + v->calc(padding()).bwidth(), bounds.cy + v->calc(1_em) };
    }

    void place(const core::rectf & box, const core::sizef & size)
    {
        ui::ViewItem::place(box, size);
        if (!button)
        {
            button = std::make_shared<ui::controls::Button>();
            button->setText(_text);
        }
        button->setShowRect({ box.topCenter(), { box.cx / 2, box.cy } });
    }

    void onFlagChanged(ui::item_flag flag, bool set)
    {
        if(flag == ui::item_flag::shown)
        {
            if(auto v = view())
            {
                if (set)
                    v->addControl(button);
                else
                    v->removeControl(button);
            }
            //button->setVisible(set);
        }
    }

private:
    std::string _text;
    drawing::wrap_mode _wrap_mode = drawing::wrap_mode::none;

    std::shared_ptr<ui::controls::Button> button;

};

void views_main()
{
    ui::shortcut sk({ ui::keybind{ ui::keycode::ctrl, ui::keycode::alt, ui::keycode::A }, ui::keybind{ ui::keycode::ctrl, ui::keycode::K } });
    auto str = sk.string();
    //std::string rtft = u8"hello لغة عربية‎𪚥𪚥བོད་ཀྱི་སྐད་ཡིག།𪚥ยิ้ยิ้ยิ😂🌐🍪🍕🚀ยิ้ยิ้ยิ้㌶㌫ ق قق ققق قققق";
    //drawing::Text shaper;
    //shaper.itermize(rtft, drawing::font(), core::colors::Black);
    //shaper.layout();
    //shaper.build(300.0f);

	auto ss = std::make_shared<ui::component::StyleSheet>();
	ss->loadFromFile("../xui/samples/test.css");

	std::shared_ptr<ui::Form> form = std::make_shared<ui::Form>(core::vec2<core::dimenf>(50_em, 30_em), ui::form_style::frameless);
    form->setStyleSheet(ss);
	form->setBorder({ 1_px, 1_px });
    form->setTitle(u8"Views");
	form->setBorderColors({ core::colors::Black, core::colors::Black });
	form->setBackgroundColor(0xffffffff);
	form->setResizeBorders({ 4_px, 4_px });
	form->setLayoutDirection(core::align::top);
	form->show();
	form->centerScreen();
	form->closed += []() { win32::endLoop(0); };

	//auto layer = std::make_shared<ui::Container>();
	//{
	//	layer->setBackgroundColor(0x40ffffff);
	//	layer->setLayoutDirection(core::align::top);
	//	layer->setLayoutOrigin(ui::layout_origin::parent);
	//	layer->setMouseThrough(true);
	//	layer->setSize({ 100_per, 100_per });
	//}

	// 标题栏
	{
		auto title = std::make_shared<ui::controlsex::TitleBar>(form);
        title->setSize({ 100_per, core::auto_value });
		title->setBackgroundColor(0xfff1f1f0);
		form->addControl(title);
	}
	{
		auto tln = std::make_shared<ui::controls::TextLine>();
		tln->setFont({ "", drawing::font().size * 4 });
		tln->setText("Hello Hello");
		tln->setSize({ 100_per, 100_per });
		tln->setImeMode(ui::ime_mode::on);
		//form->addControl(tln);
	}
    {
        auto container = std::make_shared<ui::Container>();
        //container->setSize({ core::auto_value, core::auto_value });
        container->setSize({ 100_per, 100_per });
        container->setLayoutDirection(core::align::left);
        {
            auto mv = std::make_shared<ui::MenuView>();
            mv->setBackgroundColor(0x80800080);
            //lv->setViewMode(ui::view_mode::tile);
            mv->setSize({ core::auto_value, 100_per });
            //lv->setImeMode(ui::ime_mode::on);
            for (size_t cnt = 0; cnt < 100; ++cnt)
                //lv->addItem(std::make_shared<UserViewItem>(core::format("Item ", cnt), drawing::wrap_mode::none));
            {
                auto index = mv->addItem(std::make_shared<ui::MenuViewItem>(std::make_shared <drawing::Image>("icon.png"), core::format("Item ", cnt), ui::shortcut({ ui::keybind{ ui::keycode::ctrl, ui::keycode::alt, ui::keycode::A } })));
            }

            mv->setScrollbarVisionV(ui::scrollbar_vision::always);
            container->addControl(mv);
        }

        {
            auto lv = std::make_shared<ui::ListView>();
            //lv->setViewMode(ui::view_mode::tile);
            lv->setSize({ 100_per, 100_per });
            //lv->setImeMode(ui::ime_mode::on);
            for (size_t cnt = 0; cnt < 100; ++cnt)
                //lv->addItem(std::make_shared<UserViewItem>(core::format("Item ", cnt), drawing::wrap_mode::none));
            {
                auto index = lv->addItem(std::make_shared<ui::ListViewItem>(core::format("Item ", cnt), std::make_shared <drawing::Image>("icon.png"), drawing::wrap_mode::none));
                auto item = std::dynamic_pointer_cast<ui::ListViewItem>(lv->itemAt(index));
                item->addProperty(core::format(u8"属性 ", cnt, u8" 名字不怕长，我就怕你装不ققق قققق下，你感受一下"));
                item->addProperty(core::format("Property ", cnt));
                item->addProperty(core::format("Attributes", cnt));
                item->setIconSize({ 1_em, 1_em });
            }

            container->addControl(lv);
            lv->setScrollbarVisionV(ui::scrollbar_vision::always);
        }

        form->addControl(container);
    }
    {
        std::string rtft = u8"hello لغة عربية‎𪚥𪚥བོད་ཀྱི་སྐད་ཡིག།𪚥ยิ้ยิ้ยิ้😂🌐🍪🍕🚀ยิ้ยิ้ยิ้㌶㌫ ق قق ققق قققق";
        auto tln = std::make_shared<ui::controls::TextLine>();
        tln->setFont(drawing::font("", drawing::default_font().size * 2));
        tln->setText(rtft);
        tln->setSize({ 100_per, core::auto_value });
        tln->setMargin({ 0.2_em });
        form->addControl(tln);
    }
	{
        auto container = std::make_shared<ui::Container>(core::align::left);
        container->setBackgroundColor(core::colors::LightBlue);
        container->setSize({ 100_per, core::auto_value });

        auto spacer1 = container->addSpacer(50_per);
        spacer1->setMaxSize({ 4_em, core::auto_value });
        {
            auto rbtn = std::make_shared<ui::controls::Button>();
            rbtn->setText(u8"点击这里确定");
            rbtn->setCursor(ui::cursor::hand);
            container->addControl(rbtn);
        }
        container->addSpacer(2_em);
        //container->addSpacer(50_per);
        {
            auto rbtn = std::make_shared<ui::controls::Button>();
            rbtn->setText(u8"点击这里取消");
            rbtn->setCursor(ui::cursor::wait);
            container->addControl(rbtn);
        }
        container->addSpacer(50_per);
        {
            auto rbtn = std::make_shared<ui::controls::Button>();
            rbtn->setText(u8"点击这里再次取消");
            container->addControl(rbtn);
        }
        container->addSpacer(50_per);
        {
            auto rbtn = std::make_shared<ui::controls::Button>();
            rbtn->setText(u8"点击这里再次取消");
            container->addControl(rbtn);
        }
        container->addSpacer(50_per);
        {
            auto rbtn = std::make_shared<ui::controls::Button>();
            rbtn->setText(u8"点击这里再次取消");
            container->addControl(rbtn);
            rbtn->click += [ss, &form]()
            {
                static std::shared_ptr<ui::Form> form2 = nullptr;
                if(!form2)
                {
                    form2 = std::make_shared<ui::Form>(core::vec2<core::dimenf>(50_em, 30_em), ui::form_style::frameless, form);
                    form2->setStyleSheet(ss);
                    form2->setBorder({ 1_px, 1_px });
                    form2->setTitle(u8"Popup");
                    form2->setBorderColors({ core::colors::Black, core::colors::Black });
                    form2->setBackgroundColor(0xffffffff);
                    form2->setResizeBorders({ 4_px, 4_px });
                    form2->setLayoutDirection(core::align::top);

                    // 标题栏
                    {
                        auto title = std::make_shared<ui::controlsex::TitleBar>(form2);
                        title->setSize({ 100_per, core::auto_value });
                        title->setBackgroundColor(0xfff1f1f0);
                        form2->addControl(title);
                    }
                }

                form2->show();
                form2->centerScreen();
            };
        }
        container->addSpacer(50_per);
        //container->setBorder({ 1_px, 1_px });
        //container->setBorderColors({ core::colors::Red, core::colors::Red });
        form->addControl(container);
	}
	//form->addControl(layer);
	win32::runLoop();
}
