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
    void draw(drawing::Graphics & graphics, const core::rectf & clip)
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
        return wraper.bounds() + v->calc(_padding).bsize();
    }

    void place(const core::rectf & box, const core::sizef & size)
    {
        ui::ViewItem::place(box, size);
        if(button)
            button->setShowRect({ box.topCenter(), { box.cx / 2, box.cy } });
    }

    virtual void bind(std::shared_ptr<ui::ViewFrame> frame)
    {
        ui::ViewItem::bind(frame);

        auto v = view();
        if(!button)
        {
            button = std::make_shared<ui::controls::Button>();
            button->setText(_text);
            button->setShowRect({ _box.topCenter(), { _box.cx / 2, _box.cy } });
        }

        v->addControl(button);
    }

    virtual void unbind()
    {
        if(button)
        {
            auto v = view();
            v->addControl(button);
        }
        _frame.reset();
    }
private:
    std::string _text;
    drawing::wrap_mode _wrap_mode = drawing::wrap_mode::none;

    std::shared_ptr<ui::controls::Button> button;

};

void views_main()
{
	auto ss = std::make_shared<ui::component::StyleSheet>();
	ss->loadFromFile("E:/vsrepo/xui/xui/samples/test.css");

	std::shared_ptr<ui::Form> form = std::make_shared<ui::Form>(core::vec2<core::dimensionf>(50_em, 30_em), ui::form_style::frameless);
	form->formScene()->setStyleSheet(ss);
	form->setBorder({ 1_px, 1_px });
    form->setTitle(u8"Views");
	form->setBorderColors({ core::colors::Black, core::colors::Black });
	form->setBackgroundColor(0xfffcfcfc);
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
        lv->addCreator(typeid(UserViewItem).hash_code(), []() {return std::make_shared<ui::ViewFrame>(); });
        lv->setSize({ 100_per, 100_per });
        //lv->setImeMode(ui::ime_mode::on);
        for(size_t cnt = 0; cnt < 100; ++cnt)
            lv->addItem(std::make_shared<UserViewItem>(core::format("Item ", cnt), drawing::wrap_mode::none));
        form->addControl(lv);
        lv->setScrollbarVisionV(ui::scrollbar_vision::always);
    }
	//form->addControl(layer);
	win32::runLoop();
}
