#include "stdafx.h"
#include "ListView.h"

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

        drawing::TextWraper wraper;
        wraper.itermize(_text, v->font(), core::colors::Auto);
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
    //std::string rtft = u8"hello لغة عربية‎𪚥𪚥བོད་ཀྱི་སྐད་ཡིག།𪚥ยิ้ยิ้ยิ😂🌐🍪🍕🚀ยิ้ยิ้ยิ้㌶㌫ ق قق ققق قققق";
    //drawing::TextClusterizer shaper;
    //shaper.itermize(rtft, drawing::font(), core::colors::Black);
    //shaper.layout();
    //shaper.build(300.0f);

	auto ss = std::make_shared<ui::component::StyleSheet>();
	ss->loadFromFile("../xui/samples/test.css");

	std::shared_ptr<ui::Form> form = std::make_shared<ui::Form>(core::vec2<core::dimenf>(50_em, 30_em), ui::form_style::frameless);
    auto scene = std::make_shared<ui::Scene>(form);
	scene->setStyleSheet(ss);
    form->setFormScene(scene);
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
		title->setSize({ 100_per, 1.8_em });
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
        auto lv = std::make_shared<ui::ListView>();
        lv->setViewMode(ui::view_mode::tile);
        lv->setSize({ 100_per, 100_per });
        //lv->setImeMode(ui::ime_mode::on);
        for (size_t cnt = 0; cnt < 100; ++cnt)
            //lv->addItem(std::make_shared<UserViewItem>(core::format("Item ", cnt), drawing::wrap_mode::none));
        {
            auto index =lv->addItem(std::make_shared<ui::ListViewItem>(core::format("Item ", cnt), std::make_shared <drawing::Image>("icon.png") , drawing::wrap_mode::none));
            auto item = std::dynamic_pointer_cast<ui::ListViewItem>(lv->itemAt(index));
            item->addProperty(core::format(u8"属性 ", cnt, u8" 名字不怕长，我就怕你装不ققق قققق下，你感受一下"));
            item->addProperty(core::format("Property ", cnt));
            item->setIconSize({ 1_em, 1_em });
        }

        form->addControl(lv);
        lv->setScrollbarVisionV(ui::scrollbar_vision::always);
    }
	//form->addControl(layer);
	win32::runLoop();
}
