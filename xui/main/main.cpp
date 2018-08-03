﻿#include "stdafx.h"

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
#include "controls/Image.h"
#include "controls/Container.h"
#include "controls/Text.h"
#include "controls/Form.h"
#include "controls/Desktop.h"
#include "controls/Button.h"

using namespace core;


void testImages()
{
    std::make_shared<graphics::Image>(u8"jpg/对比.jpg")->Save(string::format("out/jpg_", u8"对比", ".tga"));
    for(auto & path : io::allPaths("bmp"))
    {
        if(path.u8string().find(".bmp") != std::string::npos)
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
#include "css/parser_pp.h"


struct property_base
{
    
};

template<typename T>
struct css_property_t : public property_base
{
    std::string name;
    T value;
};


struct property_prebuild
{
    virtual ~property_prebuild() = default;

    virtual void apply(object & self)
    {

    }

    void apply(std::shared_ptr<object> ptr)
    {
        apply(*ptr);
    }
};
template<typename ClassT, typename ValT>
struct property_prebuild_instance : public property_prebuild
{
    void (ClassT::*setter)(const ValT &);
    const ValT & (ClassT::*getter)() const;
    ValT val;

    void apply(object & self_)
    {
        ClassT & self = dynamic_cast<ClassT &>(self_);
        (self.*setter)(val);
    }
};


template<typename ClassT>
struct prop_type
{
    std::string name;

    prop_type(std::string name_) : name(name_) {}
    virtual ~prop_type() = default;
    virtual std::shared_ptr<property_prebuild> build(const std::string & str) = 0;
};

template<typename ClassT, typename ValT>
struct prop_desc : public prop_type<ClassT>
{
    prop_desc(std::string name_,
    void (ClassT::*setter_)(const ValT &),
    const ValT & (ClassT::*getter_)() const,
    std::function<ValT(const std::string &)> parser_) : prop_type<ClassT>(name_), setter(setter_), getter(getter_), parser(parser_) {}

    void (ClassT::*setter)(const ValT &);
    const ValT & (ClassT::*getter)() const;

    std::function<ValT(const std::string &)> parser;

    std::shared_ptr<property_prebuild> build(const std::string & str)
    {
        auto instance = std::make_shared<property_prebuild_instance<ClassT, ValT>>();
        instance->setter = setter;
        instance->getter = getter;
        instance->val = parser(str);
        return instance;
    }
};


int parseInt(const std::string & str)
{
    return std::stoi(str);
}

class Test : public core::object
{
public:
    int val = 0;
    void set_val(const int & _val) { val = _val; }
   const int & get_val() const { return val; }

    void propertyTable(std::vector<std::shared_ptr<prop_type<Test>>> & items)
    {
        items.push_back(std::make_shared<prop_desc<Test, int>>("val", &Test::set_val, &Test::get_val, parseInt));
    }
};

int main()
{
    Test test;
    auto ptr0 = test.weak_from_this();
    std::vector<std::shared_ptr<prop_type<Test>>> items;
    test.propertyTable(items);

    for(auto & prop : items)
    {
        if(prop->name == "val")
        {
            auto pb = prop->build("456");
            pb->apply(test);
        }
    }
    std::string css = ".button { border: 1px;padding : 1em, 0.5em;} .button:hoving{border: 2px;}";
    htmlcxx::CSS::Parser parser;
    parser.parse(css);


    SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);
    win32::Win32App app;
    //testImages();

    int32_t cx = 1280;
    int32_t cy = 720;
    std::shared_ptr<graphics::Bitmap> bitmap = std::make_shared<graphics::Bitmap>(si32i{ cx, cy });
    graphics::Graphics graphics(bitmap);
    graphics.clear(colors::LightGray);

    auto dsize = controls::Desktop::instance().size();
    auto s = controls::Desktop::instance().screen(0);
    auto src = s->rect();
    auto ppi = controls::Desktop::instance().ppi();

    auto button = std::make_shared<controls::Button>(u8"点击查看更多精彩内容");
    button->setBorder({ 2_px });
    button->setBorderColors({ colors::DimGray });


    auto text = std::make_shared<controls::Text>(u8"ABCDEF这是一个很好的内容的G");
    text->setBorder({ 2_px });
    text->setBorderColors({ colors::DimGray });

    auto image0 = std::make_shared<controls::Image>("applique0.jpg");
    //image0->setBackgroundColor(colors::Green);
    image0->setImageSize({ 10_em, auto_value });
    image0->setSize({ 20_em, 30_em });
    image0->setImageFitting({ controls::renderables::image_fitting::repeat, controls::renderables::image_fitting::repeat });
    image0->setPadding({ 1_em });
    image0->setBorder({ 10_px, 20_px, 30_px, 40_px });
    //image0->setBorder({ 40_px });
    image0->setBorderColors({ colors::Blue, colors::Red, colors::DarkCyan, colors::Green });
    image0->setBorderStyles({ graphics::stroke_style::dashed });
    //image0->setBackgroundColor(colors::Red);
    image0->setMargin({ 0.5_em });
    auto image = std::make_shared<controls::Image>("applique1.jpg");
    //image->setBackgroundColor(colors::Green);
    image->setImageSize({ 10_em, auto_value });
    image->setSize({ 20_em, 20_em });
    image->setImageFitting({ controls::renderables::image_fitting::repeat, controls::renderables::image_fitting::repeat });
    image->setBorder({ 5_px });
    image->setBorderColors({ colors::Azure });

    auto text2 = std::make_shared<controls::Text>("XYZOPQRST");
    text2->setBackgroundColor(colors::Green);
    text2->setPadding({ 1_em, 0.5_em });

    auto form = std::make_shared<controls::Form>(core::vec2<core::dimensionf>(35_em, 30_em));
    form->setBorder({ 4_px });
    form->setBorderColors({colors::Black});
    form->setBorderStyles({ graphics::stroke_style::dashed});
    form->addControl(text);
    form->addControl(image0);
    form->addControl(button);
    form->addControl(image);
    form->addControl(text2);
    form->show();
    form->centerScreen();
    form->closed += []() {core::app().quit(0); };
    win32::runLoop();

#if 0
    auto scene = std::make_shared<controls::component::Scene>();
    auto container = std::make_shared<controls::Container>();
    auto row = std::make_shared<controls::Container>();
    auto text = std::make_shared<controls::Text>(u8"ABCDEF这是一个很好的内容的G");
    text->setBorder({ 2_px });
    text->setBorderColors({colors::DimGray});

    auto image0 = std::make_shared<controls::Image>("img00.png");
    //image0->setBackgroundColor(colors::Green);
    image0->setImageSize({ 10_em, auto_value });
    image0->setSize({ 20_em, 30_em });
    image0->setImageFitting({ controls::image_fitting::repeat, controls::image_fitting::repeat });
    image0->setPadding({ 1_em });
    image0->setBorder({ 10_px, 20_px, 30_px, 40_px });
    //image0->setBorder({ 40_px });
    image0->setBorderColors({ colors::Blue, colors::Red, colors::DarkCyan, colors::Green });
    image0->setBorderStyles({ graphics::stroke_style::dashed });
    //image0->setBackgroundColor(colors::Red);
    image0->setMargin({0.5_em});
    auto image = std::make_shared<controls::Image>("40612.jpg");
    //image->setBackgroundColor(colors::Green);
    image->setImageSize({ 10_em, auto_value });
    image->setSize({0.5_per, 20_em});
    image->setImageFitting({ controls::image_fitting::repeat, controls::image_fitting::repeat });
    image->setBorder({ 5_px });
    image->setBorderColors({ colors::Azure });

    auto text2 = std::make_shared<controls::Text>("XYZOPQRST");
    text2->setBackgroundColor(colors::Green);
    text2->setPadding({1_em, 0.5_em});

    row->setSize({ 1280_px, 720_px });
    row->addControl(text);
    row->addControl(image0);
    row->addControl(image);
    row->addControl(text2);
    row->enteringScene(scene);
    row->enterScene(scene);

    //auto size = row->prefferSize();
    core::si32f size = row->calc(core::vec2<core::dimensionf>(1280_px, 720_px));
    row->Control::layout(core::rc32f(core::pt32f(), size), size);
    //row->layout();
    row->update();

    counter_fps<float, 3> cps;
    //while (true)
    {
        scene->render(graphics);
        cps.acc(1);
        printf("\r%.1f", cps.fps());
    }

    //graphics.drawImage(graphics::Image("480.png"), rc32f(100, 100, 480, 270));
    bitmap->Save("scene.png");
#endif
    return 0; 
}