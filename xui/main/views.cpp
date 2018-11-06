﻿#include "stdafx.h"
#include "ListView.h"

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
        auto lv = std::make_shared<ui::controlsex::ListView>();
        lv->setSize({ 100_per, 100_per });
        //lv->setImeMode(ui::ime_mode::on);
        for(size_t cnt = 0; cnt < 100; ++cnt)
            lv->addItem(std::make_shared<ui::controlsex::ListViewItem>(core::format("Item ", cnt), drawing::wrap_mode::none));
        form->addControl(lv);
        lv->setScrollbarVisionV(ui::scrollbar_vision::always);
    }
	//form->addControl(layer);
	win32::runLoop();
}
