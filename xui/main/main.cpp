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
#include "controls/View.h"
#include "controls/Image.h"
#include "controls/Container.h"
#include "controls/Row.h"
#include "controls/Text.h"

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

int main()
{
    SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);
    win32::Win32App app;
    //testImages();

    int32_t cx = 1280;
    int32_t cy = 720;
    std::shared_ptr<graphics::Bitmap> bitmap = std::make_shared<graphics::Bitmap>(si32i{ cx, cy });
    graphics::Graphics graphics(bitmap);
    graphics.clear(colors::LightGray);

    //{
    //    counter_fps<float, 3> cps;
    //    while (true)
    //    {
    //        graphics.drawRectangle(core::rc32f(100, 100, 400, 400), graphics::PathStyle().fill(colors::Red));
    //        graphics.drawRectangle(core::rc32f(100, 100, 400, 400), graphics::PathStyle().fill(colors::Red));
    //        graphics.drawRectangle(core::rc32f(100, 100, 400, 400), graphics::PathStyle().fill(colors::Red));
    //        graphics.drawRectangle(core::rc32f(100, 100, 400, 400), graphics::PathStyle().fill(colors::Red));
    //        graphics.drawRectangle(core::rc32f(100, 100, 400, 400), graphics::PathStyle().fill(colors::Red));
    //        graphics.drawRectangle(core::rc32f(100, 100, 400, 400), graphics::PathStyle().fill(colors::Red));
    //        graphics.drawRectangle(core::rc32f(100, 100, 400, 400), graphics::PathStyle().fill(colors::Red));
    //        graphics.drawRectangle(core::rc32f(100, 100, 400, 400), graphics::PathStyle().fill(colors::Red));
    //        graphics.drawRectangle(core::rc32f(100, 100, 400, 400), graphics::PathStyle().fill(colors::Red));
    //        cps.acc(1);
    //        printf("\r%.1f", cps.fps());
    //    }
    //}

    auto scene = std::make_shared<controls::component::Scene>();
    auto container = std::make_shared<controls::Container>();
    auto row = std::make_shared<controls::Row>();
    auto text = std::make_shared<controls::Text>(u8"ABCDEF这是一个很好的内容的G");

    auto image0 = std::make_shared<controls::Image>("applique0.jpg");
    //image0->setBackgroundColor(colors::Green);
    image0->setImageSize({ 10_em, auto_value });
    image0->setSize({ 20_em, 30_em });
    image0->setImageFitting({ controls::image_fitting::repeat, controls::image_fitting::repeat });
    image0->setPadding({ 0.2_em });
    image0->setBorder({ 2_px });
    image0->setBorderColors({ colors::Blue });
    image0->setBackgroundColor(colors::Red);

    auto image = std::make_shared<controls::Image>("applique1.jpg");
    //image->setBackgroundColor(colors::Green);
    image->setImageSize({ 10_em, auto_value });
    image->setSize({20_em, 20_em});
    image->setImageFitting({ controls::image_fitting::repeat, controls::image_fitting::repeat });
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

    auto size = row->prefferSize();

    row->layout();
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
    return 0; 
}