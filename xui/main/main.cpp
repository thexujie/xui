#include "stdafx.h"

#include "core/core.h"
#include "drawing/drawing.h"
#include "ui/ui.h"

#include "win32/Win32App.h"
#include "ui/controls/Check.h"

using namespace core;


void testImages()
{
    std::make_shared<drawing::Image>(u8"jpg/对比.jpg")->Save(core::format("out/jpg_", u8"对比", ".tga"));
    for (auto & path : core::io::allPaths("bmp"))
    {
        if (path.string().find(".bmp") != std::string::npos)
            std::make_shared<drawing::Image>(path.string())->Save(core::format("out/bmp_", path.filename()));
    }
    for (auto & path : io::allPaths("tga"))
    {
        if (path.string().find(".tga") != std::string::npos)
            std::make_shared<drawing::Image>(path.string())->Save(core::format("out/tga_", path.stem(), ".tga"));
    }
    for (auto & path : io::allPaths("png"))
    {
        if (path.string().find(".png") != std::string::npos)
            std::make_shared<drawing::Image>(path.string())->Save(core::format("out/png_", path.stem(), ".tga"));
    }
    for (auto & path : io::allPaths("jpg"))
    {
        if (path.string().find(".jpg") != std::string::npos)
            std::make_shared<drawing::Image>(path.string())->Save(core::format("out/jpg_", path.stem(), ".tga"));
    }
    for (auto & path : io::allPaths("dds"))
    {
        if (path.string().find(".dds") != std::string::npos)
            std::make_shared<drawing::Image>(path.string())->Save(core::format("out/dds_", path.stem(), ".bmp"));
    }
}

#include <ShellScalingAPI.h>
#pragma comment(lib, "Shcore.lib")

//#pragma comment(lib, "E:/github/google/skia/out/x64d/skia.dll.lib")
#pragma comment(lib, "../externals/skia/bin/x64/skia.dll.lib")

class tobj
{
public:
    static void * operator new(size_t size)
    {
        return malloc(size);
    }

    static void operator delete(void *p)
    {
        free(p);
    }
};

#include "ui/controls/Radio.h"

struct tc
{
    int ab = 34;
};

std::shared_ptr<ui::Form> g_form;

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

void xui_main()
{
    //std::string rtft = u8"heသွက်ဂွံဗၠးၜးတ်ိတ်llo لغة عربية‎𪚥𪚥𪚥ยิ้ยิ้ยิ้😂🌐🍪🍕🚀ยิ้ยิ้ยิ้㌶㌫ ق قق ققق ققق";
    std::string rtft = u8"hello لغة عربية‎𪚥𪚥བོད་ཀྱི་སྐད་ཡིག།𪚥ยิ้ยิ้ยิ้😂🌐🍪🍕🚀ยิ้ยิ้ยิ้㌶㌫ ق قق ققق ققق";

    drawing::TextWraper shaper;
    shaper.itermize(u8"ยิ้ยิ้ยิ้ ", drawing::font(), colors::Black);
    shaper.layout(999999999, drawing::wrap_mode::word);

    auto ss = std::make_shared<ui::component::StyleSheet>();
    ss->loadFromFile("E:/vsrepo/xui/xui/samples/test.css");

    auto dsize = ui::Desktop::instance().size();
    auto screen = ui::Desktop::instance().screen(0);
    auto src = screen->rect();
    auto ppi = ui::Desktop::instance().ppi();

#ifdef BASE_LAYER
    auto buttons = std::make_shared<ui::Container>();
    {
        //buttons->setAlpha(0.5f);
        buttons->setLayoutDirection(core::align::top);
        for (int cnt = 0; cnt < 100; ++cnt)
        {
            auto button = std::make_shared<ui::controls::Button>(core::format(u8"点击查看更多精彩内容 ", cnt * 100));
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

    auto text = std::make_shared<ui::controls::Text>(u8"ABCDEF这是一个很好的内容的G");
    text->setBorder({ 2_px, 2_px });
    text->setBorderColors({ colors::DimGray, colors::DimGray });

    auto image0 = std::make_shared<ui::controls::Image>("applique0.jpg");
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

    auto image = std::make_shared<ui::controls::Image>("applique1.jpg");
    //image->setBackgroundColor(colors::Green);
    image->setImageSize({ 10_em, auto_value });
    image->setSize({ 20_em, 100_per });
    image->setImageFitting({ ui::image_fitting::repeat, ui::image_fitting::repeat });
    image->setBorder({ 5_px, 5_px });
    image->setBorderColors({ colors::Azure, colors::Azure });

    auto text2 = std::make_shared<ui::controls::Text>("XYZOPQRST");
    text2->setBackgroundColor(colors::Green);
    text2->setPadding({ 1_em, 0.5_em });
#endif

    std::shared_ptr<ui::Form> form = std::make_shared<ui::Form>(core::vec2<core::dimensionf>(50_em, 30_em), ui::form_style::normal);
    g_form = form;
    form->formScene()->setStyleSheet(ss);
    form->setBorder({ 1_px, 1_px });
    form->setBorderColors({ colors::Black, colors::Black });
#ifdef BASE_LAYER
    form->addControl(text);
    form->addControl(image0);
    form->addControl(scrollbar);
    form->addControl(buttons);
    form->addControl(image);
    form->addControl(text2);
#endif
    form->show();
    form->centerScreen();
    form->closed += []() { win32::endLoop(0); };
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

        auto tln = std::make_shared<ui::controls::TextLine>();
        tln->setFont({"", font.size * 4});
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
        tln->setSize({ auto_value, 10_em });
        tln->setFont({ "", font.size * 9 });
        tln->setAnchor({ 0_px, 0_px, 0_px, 0_px });
        tln->setAnchorBorders(core::align::leftRight);
        tln->setImeMode(ui::ime_mode::on);
        layer->addControl(tln);
    }
    {
        auto grid = std::make_shared<ui::Grid>();
        grid->setSize({ 100_per, auto_value });
        grid->setRows({ 100_per });
        grid->setCols({ 33.333_per , 33.333_per , 33.333_per });
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
            rbtn->setGroup("abcd");
            container->addControl(rbtn);
        }
        {
            auto rbtn = std::make_shared<ui::controls::Radio>();
            rbtn->setText(u8"将数据放置于尾部，便于实时查看");
            rbtn->setGroup("abcd");
            container->addControl(rbtn);
        }
        {
            auto rbtn = std::make_shared<ui::controls::Radio>();
            rbtn->setText(u8"Lucccccccccy");
            rbtn->setGroup("abcd");
            container->addControl(rbtn);
        }
        {
            auto rbtn = std::make_shared<ui::controls::Radio>();
            rbtn->setText(u8"Vickyyyyyyy");
            rbtn->setGroup("abcd");
            container->addControl(rbtn);
        }
        layer->addControl(container);
        container->setBorder({ 1_px, 1_px });
        container->setBorderColors({ colors::Red , colors::Red });
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
            rbtn->stateChanged += [](ui::check_state state) { win32::endLoop(0); };
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
            container->addControl(rbtn);
            rbtn->stateChanged += [](ui::check_state state) {setStyle(ui::form_style::nomin, state == ui::check_state::unchecked); };
        }
        {
            auto rbtn = std::make_shared<ui::controls::Check>();
            rbtn->setText(u8"max box");
            container->addControl(rbtn);
            rbtn->stateChanged += [](ui::check_state state) {setStyle(ui::form_style::nomax, state == ui::check_state::unchecked); };
        }
        {
            auto rbtn = std::make_shared<ui::controls::Radio>();
            rbtn->setText(u8"normal");
            rbtn->setGroup("window style");
            container->addControl(rbtn);
            rbtn->stateChanged += [](ui::check_state state) {setFormType(ui::form_style::normal); };
        }
        {
            auto rbtn = std::make_shared<ui::controls::Radio>();
            rbtn->setText(u8"frameless");
            rbtn->setGroup("window style");
            container->addControl(rbtn);
            rbtn->stateChanged += [](ui::check_state state) {setFormType(ui::form_style::frameless); };
        }
        {
            auto rbtn = std::make_shared<ui::controls::Radio>();
            rbtn->setText(u8"layered");
            rbtn->setGroup("window style");
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
        container->setCompactLayout(true);
        container->setSize({ 100_per, auto_value });

        auto spacer1 = container->addSpacer(50_per);
        spacer1->setMaxSize({4_em, core::auto_value});
        {
            auto rbtn = std::make_shared<ui::controls::Button>();
            rbtn->setText(u8"点击这里确定");
            container->addControl(rbtn);
        }
        container->addSpacer(2_em);
        //container->addSpacer(50_per);
        {
            auto rbtn = std::make_shared<ui::controls::Button>();
            rbtn->setText(u8"点击这里取消");
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

    auto app = std::make_shared<win32::Win32App>();
    xui_main();
    return 0;
}
