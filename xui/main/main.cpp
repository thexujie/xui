#include "stdafx.h"

#include "core/app.h"
#include "core/counter_fps.h"
#include "core/vec2.h"
#include "graphics/GraphicsService.h"
#include "graphics/Graphics.h"
#include "win32/Bitmap.h"
#include "win32/windows.h"
#include "win32/GraphicsService.h"
#include "win32/Win32App.h"
#include "graphics/Image.h"
#include "core/io/filestream.h"
#include "win32/uniscribe/script.h"
#include "graphics/TextBlob.h"
#include "ui/Container.h"
#include "ui/Form.h"
#include "ui/Desktop.h"
#include "ui/controls/Text.h"
#include "ui/controls/Image.h"
#include "ui/controls/Button.h"
#include "ui/controls/ScrollBar.h"
#include "ui/controls/TextBox.h"

using namespace core;


void testImages()
{
    std::make_shared<graphics::Image>(u8"jpg/对比.jpg")->Save(string::format("out/jpg_", u8"对比", ".tga"));
    for (auto & path : io::allPaths("bmp"))
    {
        if (path.u8string().find(".bmp") != std::string::npos)
            std::make_shared<graphics::Image>(path.u8string())->Save(string::format("out/bmp_", path.filename()));
    }
    for (auto & path : io::allPaths("tga"))
    {
        if (path.u8string().find(".tga") != std::string::npos)
            std::make_shared<graphics::Image>(path.u8string())->Save(string::format("out/tga_", path.stem(), ".tga"));
    }
    for (auto & path : io::allPaths("png"))
    {
        if (path.u8string().find(".png") != std::string::npos)
            std::make_shared<graphics::Image>(path.u8string())->Save(string::format("out/png_", path.stem(), ".tga"));
    }
    for (auto & path : io::allPaths("jpg"))
    {
        if (path.u8string().find(".jpg") != std::string::npos)
            std::make_shared<graphics::Image>(path.u8string())->Save(string::format("out/jpg_", path.stem(), ".tga"));
    }
    for (auto & path : io::allPaths("dds"))
    {
        if (path.u8string().find(".dds") != std::string::npos)
            std::make_shared<graphics::Image>(path.u8string())->Save(string::format("out/dds_", path.stem(), ".bmp"));
    }
}

#include <ShellScalingAPI.h>
#pragma comment(lib, "Shcore.lib")

//#pragma comment(lib, "E:/github/google/skia/out/x64d/skia.dll.lib")
#pragma comment(lib, "../externals/skia/bin/x64/skia.dll.lib")

void xui_main()
{
    std::string str = ".button { border: 1px; padding : 1em, 0.5em;background-color:#2888} .button hoving{border: 2px; background-color:#a888}";
    auto ss = std::make_shared<ui::component::StyleSheet>();
    ss->loadFromFile("E:/vsrepo/xui/xui/samples/test.css");

    auto dsize = ui::Desktop::instance().size();
    auto screen = ui::Desktop::instance().screen(0);
    auto src = screen->rect();
    auto ppi = ui::Desktop::instance().ppi();

    auto buttons = std::make_shared<ui::Container>();
    {
        buttons->setLayoutDirection(core::align::top);
        for (int cnt = 0; cnt < 100; ++cnt)
        {
            auto button = std::make_shared<ui::controls::Button>(core::string::format(u8"点击查看更多精彩内容 ", cnt * 100));
            button->setBorder({ 2_px });
            button->setBorderColors({ colors::DimGray });
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

    auto text = std::make_shared<ui::controls::Text>(u8"ABCDEF这是一个很好的内容的G");
    text->setBorder({ 2_px });
    text->setBorderColors({ colors::DimGray });

    auto image0 = std::make_shared<ui::controls::Image>("applique0.jpg");
    //image0->setBackgroundColor(colors::Green);
    image0->setImageSize({ 10_em, auto_value });
    image0->setSize({ 20_em, 30_em });
    image0->setImageFitting({ ui::renderables::image_fitting::repeat, ui::renderables::image_fitting::repeat });
    image0->setPadding({ 1_em });
    image0->setBorder({ 10_px, 20_px, 30_px, 40_px });
    //image0->setBorder({ 40_px });
    image0->setBorderColors({ colors::Blue, colors::Red, colors::DarkCyan, colors::Green });
    image0->setBorderStyles({ graphics::stroke_style::dashed });
    //image0->setBackgroundColor(colors::Red);
    image0->setMargin({ 0.5_em });

    auto scrollbar = std::make_shared<ui::controls::ScrollBar>();
    //image0->setBackgroundColor(colors::Green);
    scrollbar->setSize({ 1_em, 100_per });

    auto image = std::make_shared<ui::controls::Image>("applique1.jpg");
    //image->setBackgroundColor(colors::Green);
    image->setImageSize({ 10_em, auto_value });
    image->setSize({ 20_em, 100_per });
    image->setImageFitting({ ui::renderables::image_fitting::repeat, ui::renderables::image_fitting::repeat });
    image->setBorder({ 5_px });
    image->setBorderColors({ colors::Azure });

    auto text2 = std::make_shared<ui::controls::Text>("XYZOPQRST");
    text2->setBackgroundColor(colors::Green);
    text2->setPadding({ 1_em, 0.5_em });

    auto form = std::make_shared<ui::Form>(core::vec2<core::dimensionf>(50_em, 30_em));
    form->formScene()->setStyleSheet(ss);
    form->setBorder({ 1_px });
    form->setBorderColors({ colors::Black });
    //form->setBorderStyles({ graphics::stroke_style::dashed });
    form->addControl(text);
    form->addControl(image0);
    form->addControl(scrollbar);
    form->addControl(buttons);
    form->addControl(image);
    form->addControl(text2);
    form->show();
    form->centerScreen();
    form->closed += []() { win32::endLoop(0); };
    //form->setScrollbarVisionV(ui::scrollbar_vision::always);
    form->setScrollbarVisionH(ui::scrollbar_vision::always);
    const core::property_table & props = core::app().properties<ui::controls::Button>();

    graphics::font font;
    graphics::fontmetrics fm(font);

    auto layer = std::make_shared<ui::Container>();
    {
        layer->setBackgroundColor(0x40ffffff);
        layer->setLayoutDirection(core::align::top);
        layer->setLayoutOrigin(ui::layout_origin::parent);
        layer->setMouseThrough(true);
        layer->setSize({ 100_per });

        auto tbx = std::make_shared<ui::controls::TextBox>();
        tbx->setSize({ 100_per, 2_em });
        tbx->setImeMode(ui::ime_mode::on);
        layer->addControl(tbx);

        form->addControl(layer);
    }
    {
        auto tbx = std::make_shared<ui::controls::TextBox>();
        tbx->setSize({ 100_per, 2_em });
        layer->addControl(tbx);

        form->addControl(layer);
    }

    //auto inter = [](const core::color32 & s, const core::color32 & e, float32_t inter)-> core::color32
    //{
    //    auto a = s.a + (e.a - s.a) * inter;
    //    auto r = s.r + (e.r - s.r) * inter;
    //    auto g = s.g + (e.g - s.g) * inter;
    //    auto b = s.b + (e.b - s.b) * inter;
    //    return core::color32(a, r, g, b);
    //};

    //auto prop = props.at("background-color");
    core::timer t(10ms);
    std::vector<std::shared_ptr<animation>> anims;
    //for (auto & iter : buttons->children())
    //{
    //    auto& control = iter.second;
    //    if (!control->is_type_of<ui::controls::Button>())
    //        continue;

    //    auto interpolator = make_interpolator<core::color32>(inter);
    //    prop->serialize("red", interpolator->start());
    //    prop->serialize("green", interpolator->end());

    //    auto anim = std::make_shared<property_animation>(control, prop, interpolator);
    //    anim->setDuration(3s);
    //    anim->stoped += []() {printf("ended \n"); };
    //    anims.push_back(anim);
    //}

    {
        auto & props2 = core::app().properties<ui::Form>();

        auto prop = props2.at("size");
        auto interpolator = std::make_shared<property_interpolator_default<core::vec2<core::dimensionf>>>();
        prop->serialize("50em 30em", interpolator->start());
        prop->serialize("50em 50em", interpolator->end());

        auto anim = std::make_shared<property_animation>(form, prop, interpolator);
        anim->setDuration(1s);
        //anim->setLoop(100);
        anim->stoped += []() {printf("form resize done! \n"); };
        anims.push_back(anim);
    }

    for (auto & a : anims)
    {
        a->start();
    }
    t.tick += [&anims](core::timer &, int64_t)
    {
        for (auto & a : anims)
        {
            a->update();
        }
    };

    t.start();

    win32::runLoop();
}

int main()
{
    SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);

    graphics::StringFormat fmt;
    auto textblob = std::make_shared<graphics::TextBlob>("abcdefg", fmt);

    auto app = std::make_shared<win32::Win32App>();
    xui_main();
    return 0;
}
