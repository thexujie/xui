#include "stdafx.h"

using namespace core;


static std::shared_ptr<ui::Form> g_form;

void setFormType(ui::form_style style)
{
	auto styles = g_form->styles();
	styles.set(ui::form_style::mask_type, false);
	styles.set(style, true);
	g_form->setStyles(styles);
}

void setStyle(ui::form_style style, bool b)
{
	auto styles = g_form->styles();
	styles.set(style, b);
	g_form->setStyles(styles);
}

#define BASE_LAYER

void all_main()
{
	//std::string rtft = u8"heသွက်ဂွံဗၠးၜးတ်ိတ်llo لغة عربية‎𪚥𪚥𪚥ยิ้ยิ้ยิ้😂🌐🍪🍕🚀ยิ้ยิ้ยิ้㌶㌫ ق قق ققق ققق";
	std::u8string rtft = u8"hello لغة عربية‎𪚥𪚥བོད་ཀྱི་སྐད་ཡིག།𪚥ยิ้ยิ้ยิ้😂🌐🍪🍕🚀ยิ้ยิ้ยิ้㌶㌫ ق قق ققق قققق";

	drawing::TextWraper shaper(u8"ยิ้ยิ้ยิ ق قق ققق قققق");
	shaper.itermize(drawing::font(), colors::Black);
	shaper.layout(999999999, drawing::wrap_mode::word);

	auto ss = std::make_shared<ui::component::StyleSheet>();
    ss->loadFromFile(u8"../xtest/samples/test.css");

	auto dsize = ui::Desktop::instance().size();
	auto screen = ui::Desktop::instance().screen(0);
	auto src = screen->rect();
	auto ppi = ui::Desktop::instance().ppi();

#ifdef BASE_LAYER
	auto buttons = std::make_shared<ui::Container>();
	{
		//buttons->setAlpha(0.5f);
		buttons->setLayoutDirection(core::align::top);
		for(int cnt = 0; cnt < 100; ++cnt)
		{
			//auto button = std::make_shared<ui::controls::Button>(core::format(u8"点击查看更多精彩内容 ", cnt * 100));
			auto button = std::make_shared<ui::controls::Button>(u8"点击查看更多精彩内容 ");
			button->setBorder({ 2_px, 2_px });
			button->setBorderColors({ colors::DimGray, colors::DimGray });
			buttons->addControl(button);
		}

		auto scrollbar2 = std::make_shared<ui::controls::ScrollBar>();
		//image0->setBackgroundColor(colors::Green);
		scrollbar2->setDirection(core::align::left);
		scrollbar2->setSize({ 100_per, 1_em });
		buttons->addControl(scrollbar2);
		buttons->setBackgroundColor(0xffffffff);
		buttons->setScrollbarVisionV(ui::scrollbar_vision::always);
		buttons->setSize({ auto_value, 100_per });
	}

	std::basic_ostringstream<char8_t> oss;
	oss << u8"dawd";

	auto text = std::make_shared<ui::controls::Lable>();
    text->setText(u8"ABCDEF这是一个很好的内容的G");
	text->setBorder({ 2_px, 2_px });
	text->setBorderColors({ colors::DimGray, colors::DimGray });

	auto image0 = std::make_shared<ui::controls::Image>(u8"applique0.jpg");
	//image0->setBackgroundColor(colors::Green);
	image0->setImageSize({ 10_em, auto_value });
	image0->setSize({ 20_em, 30_em });
	image0->setImageFitting({ ui::image_fitting::repeat, ui::image_fitting::repeat });
	image0->setPadding({ 1_em, 1_em });
	image0->setBorder({ 10_px, 20_px, 30_px, 40_px });
	//image0->setBorder({ 40_px });
	image0->setBorderColors({ colors::Blue, colors::Red, colors::DarkCyan, colors::Green });
	image0->setBorderStyles({ drawing::stroke_style::dashed, drawing::stroke_style::dashed });
	//image0->setBackgroundColor(colors::Red);
	image0->setMargin({ 0.5_em, 0.5_em });

	auto scrollbar = std::make_shared<ui::controls::ScrollBar>();
	//image0->setBackgroundColor(colors::Green);
	scrollbar->setSize({ 1_em, 100_per });

	auto image = std::make_shared<ui::controls::Image>(u8"applique1.jpg");
	//image->setBackgroundColor(colors::Green);
	image->setImageSize({ 10_em, auto_value });
	image->setSize({ 20_em, 100_per });
	image->setImageFitting({ ui::image_fitting::repeat, ui::image_fitting::repeat });
	image->setBorder({ 5_px, 5_px });
	image->setBorderColors({ colors::Azure, colors::Azure });

	auto text2 = std::make_shared<ui::controls::Lable>();
    text2->setText(u8"XYZOPQRST");
	text2->setBackgroundColor(colors::Green);
	text2->setPadding({ 1_em, 0.5_em });
#endif

	std::shared_ptr<ui::Form> form = std::make_shared<ui::Form>(core::vec2<core::dimenf>(50_em, 30_em), ui::form_style::normal);
	g_form = form;
    form->setStyleSheet(ss);
	form->setBorder({ 1_px, 1_px });
	form->setBorderColors({ colors::Black, colors::Black });
	form->setBackgroundColor(0x80ffffff);
	form->setResizeBorders({ 4_px, 4_px });
#ifdef BASE_LAYER
	form->addControl(text);
	form->addControl(image0);
	form->addControl(scrollbar);
	//form->addControl(buttons);
	form->addControl(image);
	form->addControl(text2);
#endif
	form->show();
	form->centerScreen();
	form->closed += []() { core::app().quit(0); };
	form->setScrollbarVisionH(ui::scrollbar_vision::always);
	const core::property_table & props = core::app().properties<ui::controls::Button>();

	drawing::font font;
	drawing::fontmetrics fm(font);

	auto layer = std::make_shared<ui::Container>();
	{
		layer->setBackgroundColor(0x40ffffff);
		layer->setLayoutDirection(core::align::top);
		layer->setLayoutOrigin(ui::layout_origin::parent);
		layer->setMouseThrough(true);
		layer->setSize({ 100_per, 100_per });
	}

	// 标题栏
	{
		auto title = std::make_shared<ui::controlsex::TitleBar>(form);
		title->setSize({ 100_per, core::auto_value });
		title->setBackgroundColor(0xfff1f1f0);
		layer->addControl(title);
	}
	{
		auto tln = std::make_shared<ui::controls::TextLine>();
		tln->setFont({ u8"", font.size * 4 });
		tln->setText(u8"သွက်ဂွံဗၠးၜးတ်ိတ်");
		tln->setSize({ 100_per, auto_value });
		tln->setImeMode(ui::ime_mode::on);
		layer->addControl(tln);
	}
	{
		auto tln = std::make_shared<ui::controls::TextLine>();
		tln->setText(rtft);
		tln->setSize({ 100_per, auto_value });
		layer->addControl(tln);
	}
	{
		auto tln = std::make_shared<ui::controls::TextLine>();
		tln->setText(rtft);
		//tln->setSize({ auto_value, 10_em });
		tln->setFont({ u8"", font.size * 9 });
        tln->setAnchor({ 0, core::nanf32, 0, core::nanf32 });
		tln->setImeMode(ui::ime_mode::on);
		layer->addControl(tln);
	}
	{
		auto grid = std::make_shared<ui::Grid>();
		grid->setSize({ 100_per, auto_value });
		grid->setRows({ 100_per });
		grid->setCols({ 33.333_per, 33.333_per, 33.333_per });
		{
			auto tln = std::make_shared<ui::controls::TextLine>();
			tln->setText(rtft);
			tln->setSize({ auto_value, auto_value });
			tln->setImeMode(ui::ime_mode::on);
			grid->addControl(0, 0, tln);
		}
		{
			auto tln = std::make_shared<ui::controls::TextLine>();
			tln->setText(rtft);
			tln->setSize({ auto_value, auto_value });
			tln->setImeMode(ui::ime_mode::on);
			grid->addControl(0, 1, tln);
		}
		{
			auto tln = std::make_shared<ui::controls::TextLine>();
			tln->setText(rtft);
			tln->setSize({ auto_value, auto_value });
			tln->setImeMode(ui::ime_mode::on);
			grid->addControl(0, 2, tln);
		}

		layer->addControl(grid);
	}
	{
		auto container = std::make_shared<ui::Container>();
		container->setLayoutDirection(core::align::top);
		container->setSize({ 100_per, auto_value });

		{
			auto rbtn = std::make_shared<ui::controls::Radio>();
			rbtn->setText(u8"将数据放置于上方");
			rbtn->setGroup(u8"abcd");
			container->addControl(rbtn);
		}
		{
			auto rbtn = std::make_shared<ui::controls::Radio>();
			rbtn->setText(u8"将数据放置于尾部，便于实时查看");
			rbtn->setGroup(u8"abcd");
			container->addControl(rbtn);
		}
		{
			auto rbtn = std::make_shared<ui::controls::Radio>();
			rbtn->setText(u8"Lucccccccccy");
			rbtn->setGroup(u8"abcd");
			container->addControl(rbtn);
		}
		{
			auto rbtn = std::make_shared<ui::controls::Radio>();
			rbtn->setText(u8"Vickyyyyyyy");
			rbtn->setGroup(u8"abcd");
			container->addControl(rbtn);
		}
		layer->addControl(container);
		container->setBorder({ 1_px, 1_px });
		container->setBorderColors({ colors::Red, colors::Red });
	}
	{
		auto container = std::make_shared<ui::Container>();
		container->setLayoutDirection(core::align::top);
		container->setSize({ 100_per, auto_value });

		{
			auto rbtn = std::make_shared<ui::controls::Check>();
			rbtn->setText(u8"将数据放置于上方");
			container->addControl(rbtn);
		}
		{
			auto rbtn = std::make_shared<ui::controls::Check>();
			rbtn->setText(u8"将数据放置于尾部，便于实时查看");
			container->addControl(rbtn);
		}
		{
			auto rbtn = std::make_shared<ui::controls::Check>();
			rbtn->setText(u8"Lucccccccccy");
			container->addControl(rbtn);
		}
		{
			auto rbtn = std::make_shared<ui::controls::Check>();
			rbtn->setText(u8"Vickyyyyyyy");
			container->addControl(rbtn);
			rbtn->stateChanged += [](ui::check_state state) { core::app().quit(0); };
		}
		layer->addControl(container);
		container->setBorder({ 1_px, 1_px });
		container->setBorderColors({ colors::Red, colors::Red });
	}
	{
		auto container = std::make_shared<ui::Container>();
		container->setLayoutDirection(core::align::top);
		container->setSize({ 100_per, auto_value });

		{
			auto rbtn = std::make_shared<ui::controls::Check>();
			rbtn->setText(u8"min box");
			rbtn->setCheckState(ui::check_state::checked);
			container->addControl(rbtn);
			rbtn->stateChanged += [](ui::check_state state) {setStyle(ui::form_style::nomin, state == ui::check_state::unchecked); };
		}
		{
			auto rbtn = std::make_shared<ui::controls::Check>();
			rbtn->setText(u8"max box");
			rbtn->setCheckState(ui::check_state::checked);
			container->addControl(rbtn);
			rbtn->stateChanged += [](ui::check_state state) {setStyle(ui::form_style::nomax, state == ui::check_state::unchecked); };
		}
		{
			auto rbtn = std::make_shared<ui::controls::Check>();
			rbtn->setText(u8"resizable");
			rbtn->setCheckState(ui::check_state::checked);
			container->addControl(rbtn);
			rbtn->stateChanged += [](ui::check_state state) {setStyle(ui::form_style::noresizable, state == ui::check_state::unchecked); };
		}
		{
			auto rbtn = std::make_shared<ui::controls::Check>();
			rbtn->setText(u8"no task button");
			container->addControl(rbtn);
			rbtn->stateChanged += [](ui::check_state state) {setStyle(ui::form_style::notaskbutton, state == ui::check_state::checked); };
		}
		{
			auto rbtn = std::make_shared<ui::controls::Check>();
			rbtn->setText(u8"top most");
			container->addControl(rbtn);
			rbtn->stateChanged += [](ui::check_state state) {setStyle(ui::form_style::topmost, state == ui::check_state::checked); };
		}
		{
			auto rbtn = std::make_shared<ui::controls::Radio>();
			rbtn->setText(u8"normal");
			rbtn->setGroup(u8"window style");
			rbtn->setCheckState(ui::check_state::checked);
			container->addControl(rbtn);
			rbtn->stateChanged += [](ui::check_state state) {setFormType(ui::form_style::normal); };
		}
		{
			auto rbtn = std::make_shared<ui::controls::Radio>();
			rbtn->setText(u8"frameless");
			rbtn->setGroup(u8"window style");
			container->addControl(rbtn);
			rbtn->stateChanged += [](ui::check_state state) {setFormType(ui::form_style::frameless); };
		}
		{
			auto rbtn = std::make_shared<ui::controls::Radio>();
			rbtn->setText(u8"layered");
			rbtn->setGroup(u8"window style");
			container->addControl(rbtn);
			rbtn->stateChanged += [](ui::check_state state) {setFormType(ui::form_style::layered); };
		}
		layer->addControl(container);
		container->setBorder({ 1_px, 1_px });
		container->setBorderColors({ colors::Red, colors::Red });
	}
	{
		auto container = std::make_shared<ui::Container>(core::align::left);
		container->setBackgroundColor(core::colors::LightBlue);
		container->setSize({ 100_per, auto_value });

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
		}
		container->addSpacer(50_per);
		layer->addControl(container);
		container->setBorder({ 1_px, 1_px });
		container->setBorderColors({ colors::Red, colors::Red });
	}
	form->addControl(layer);

	//auto inter = [](const core::color & s, const core::color & e, float32_t inter)-> core::color
	//{
	//    auto a = s.a + (e.a - s.a) * inter;
	//    auto r = s.r + (e.r - s.r) * inter;
	//    auto g = s.g + (e.g - s.g) * inter;
	//    auto b = s.b + (e.b - s.b) * inter;
	//    return core::color(a, r, g, b);
	//};

	//auto prop = props.at("background-color");
	core::timer t(10ms);
	std::vector<std::shared_ptr<animation>> anims;
	//for (auto & iter : buttons->children())
	//{
	//    auto& control = iter.second;
	//    if (!control->is_type_of<ui::controls::Button>())
	//        continue;

	//    auto interpolator = make_interpolator<core::color>(inter);
	//    prop->serialize("red", interpolator->start());
	//    prop->serialize("green", interpolator->end());

	//    auto anim = std::make_shared<property_animation>(control, prop, interpolator);
	//    anim->setDuration(3s);
	//    anim->stoped += []() {printf("ended \n"); };
	//    anims.push_back(anim);
	//}

	{
		auto & props2 = core::app().properties<ui::Form>();

		auto prop = props2.at(u8"size");
		auto interpolator = std::make_shared<property_interpolator_default<core::vec2<core::dimenf>>>();
		prop->serialize(u8"50em 30em", interpolator->start());
		prop->serialize(u8"50em 50em", interpolator->end());

		auto anim = std::make_shared<property_animation>(form, prop, interpolator);
		anim->setDuration(1s);
		//anim->setLoop(100);
		anim->stoped += []() {printf("form resize done! \n"); };
		anims.push_back(anim);
	}

	for(auto & a : anims)
	{
		a->start();
	}
	t.tick += [&anims](core::timer &, int64_t)
	{
		for(auto & a : anims)
		{
			a->update();
		}
		//g_form->centerScreen();
	};

	t.start();
	core::app().loop();
}
