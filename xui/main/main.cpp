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
#include "controls/Container.h"
#include "controls/Image.h"

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

    auto container = std::make_shared<controls::Container>();
    auto scene = std::make_shared<controls::component::Scene>();
    auto image = std::make_shared<controls::Image>("960.png");

    container->addControl(image);
    container->addControl(image);
    container->enteringScene(scene);
    container->enterScene(scene);

    scene->render(graphics);
    bitmap->Save("scene.png");

    //auto image = std::make_shared<graphics::Image>("lcw.tga");
    //graphics.DrawImage(*(image.get()), { cx / 4, cy / 4 }, align::leftTop);
    //graphics.DrawImage(*image, { 160, 10 }, align::leftTop);
    //graphics.DrawImage(*image, { 200, 20 }, align::leftTop);
    //graphics.DrawImage(*image, { 200, 20, 400, 100 });
    //graphics.DrawImage(*image, rc32i{100, 100, 500, 100}, rc32i{ 100, 0, 194, 215});

    //graphics.DrawImage(*(image.get()), { 0, 0, cx / 4, cy / 4 }, align::bottomCenterX);
    //graphics.DrawImage(*(image.get()), { cx / 4, 0, cx / 4, cy / 4 });

    //graphics.DrawImage(*(image.get()), { 0, 0, cx / 2, cy / 4 }, align::bottomCenterX);

    //graphics.DrawImage(*(image.get()), { cx / 4, cy / 4 }, { 20, 300, 600, 200 }, align::leftTop);

    //graphics.DrawImage(*(image.get()), { cx / 4, cy / 4, cx / 2, cy / 2 }, { 20, 300, 600, 200 });
    //graphics.drawLine({ cx / 2, cy }, { cx / 2, 0 },  colors::Yellow, 1);

    graphics.drawLine(pt32f(0, cy / 2), pt32f(cx, cy / 2), graphics::PathStyle().stoke(colors::Red));
    graphics.drawLine(pt32f(cx / 2, 0), pt32f(cx / 2, cy), graphics::PathStyle().stoke(colors::Red));
    graphics.drawLine(pt32f(0, 0), pt32f(cx, cy), graphics::PathStyle().stoke(colors::Red));
    graphics.drawLine(pt32f(cx, 0), pt32f(0, cy), graphics::PathStyle().stoke(colors::Red));



    graphics.drawImage(graphics::Image("960.png"), rc32f(cx / 4, cy / 4, cx / 2, cy / 2));
    //graphics.drawImage(graphics::Image("1920.png"), rc32f(cx / 4, cy / 4, cx / 2, cy / 2));
    graphics.drawEllipse(rc32f(0, 0, cx, cy), graphics::PathStyle(graphics::PathMode::Stroke).stoke(colors::Red).width(2));
    graphics.drawEllipse(rc32f(cx / 4, cy / 4, cx / 2, cy / 2), graphics::PathStyle(graphics::PathMode::StrokeFill).stoke(0x4000ff00).width(2));

    graphics.drawRoundRect(rc32f( cx / 4, cy / 4, cx / 2, cy / 2 ), 50, 50, graphics::PathStyle().stoke(colors::Green).width(4));

    //graphics.drawString(u8"一二三四五六七八九十", pt32f( cx / 2, cy / 2), graphics::StringFormat({ "", 16 }).color(colors::Red).weight(graphics::text::Weight_ExtraBlackBold).align(core::align::rightBottom));
    //graphics.drawString(u8"一二三四五六七八九十", pt32f( cx / 2, cy / 2), graphics::StringFormat({ "", 16 }).color(colors::Green).weight(graphics::text::Weight_Bold).align(core::align::leftBottom));
    //graphics.drawString(u8"一二三四五六七八九十", pt32f( cx / 2, cy / 2), graphics::StringFormat({ "", 16 }).color(colors::Blue).weight(graphics::text::Weight_Bold).align(core::align::leftTop));
    //graphics.drawString(u8"一二三四五六七八九十", pt32f( cx / 2, cy / 2), graphics::StringFormat({ "", 16 }).color(colors::PeachPuff).weight(graphics::text::Weight_Bold).align(core::align::rightTop));


    //graphics.DrawString(u8"LTLT", colors::Black, { "", 10 }, { cx / 2, cy / 2 }, align::leftTop);
    //graphics.DrawString(u8"RTRT", colors::Gray, { "", 10 }, { cx / 2, cy / 2 }, align::rightTop);
    //graphics.DrawString(u8"RBRB", colors::Green, { "", 10 }, { cx / 2, cy / 2 }, align::rightBottom);
    //graphics.DrawString(u8"LBLB", colors::Blue, { "", 10 }, { cx / 2, cy / 2 }, align::leftBottom);

    //graphics.drawString(u8"LTLT", { cx / 2, cy / 2 }, { "", 20 }, colors::Black, align::leftCenterY);
    //graphics.drawString(u8"RTRT", { cx / 2, cy / 2 }, { "", 20 }, colors::Gray, align::topCenterX);
    //graphics.drawString(u8"RBRB", { cx / 2, cy / 2 }, { "", 20 }, colors::Green, align::rightCenterY);
    //graphics.drawString(u8"LBLB", { cx / 2, cy / 2 }, { "", 20 }, colors::Blue, align::bottomCenterX);

    //graphics.drawString(u8"LTLT", rc32f(cx / 4, cy / 4, cx / 2, cy / 2), { "", 20 }, colors::Red, align::leftTop);
    //graphics.drawString(u8"RTRT", rc32f(cx / 4, cy / 4, cx / 2, cy / 2), { "", 20 }, colors::Yellow, align::rightTop);
    //graphics.drawString(u8"RBRB", rc32f(cx / 4, cy / 4, cx / 2, cy / 2), { "", 20 }, colors::Green, align::rightBottom);
    //graphics.drawString(u8"LBLB", rc32f(cx / 4, cy / 4, cx / 2, cy / 2), { "", 20 }, colors::Blue, align::leftBottom);

    //graphics.drawString(u8"LTLT", rc32f(cx / 4, cy / 4, cx / 2, cy / 2), { "", 20 }, colors::Red, align::leftCenterY);
    //graphics.drawString(u8"RTRT", rc32f(cx / 4, cy / 4, cx / 2, cy / 2), { "", 20 }, colors::Yellow, align::topCenterX);
    //graphics.drawString(u8"RBRB", rc32f(cx / 4, cy / 4, cx / 2, cy / 2), { "", 20 }, colors::Green, align::rightCenterY);
    //graphics.drawString(u8"LBLB", rc32f(cx / 4, cy / 4, cx / 2, cy / 2), { "", 20 }, colors::Blue, align::bottomCenterX);

    //auto cm = std::make_shared<graphics::Image>("lcw.tga");
    //cm->Save("cm.bmp");
    //graphics.DrawImage(*cm.get(), { 10, 10 }, 0);
    //graphics.DrawImage(*cm.get(), { 50, 50 }, 0);
    //graphics.drawString(u8"各位啊两个金额哇啦关雎尔挖掘各位甲骨文阿留个空位过了旺季安哥拉晋文公来围观", { 10, 10, 300, 100 }, { "", 40, 0, graphics::text::font::cleartype }, colors::Black, align::leftTop);
    //graphics.drawString(u8"各位啊两个金额哇啦关雎尔挖掘各位甲骨文阿留个空位过了旺季安哥拉晋文公来围观", { 10, 50, 300, 100 }, { "", 40, 0, graphics::text::font::gray }, colors::Black, align::leftTop);
    //auto si = graphics.MeasureString(u8"各位啊两个金额哇啦关雎尔挖掘各位甲骨文阿留个空\r\n位过了旺季安哥拉晋文公来围观", {"", 10 });

    //graphics.drawRectangle({ 100, 100, 200, 50 }, graphics::PathStyle().stoke(colors::Red));
    //graphics.drawRectangle({ 100, 220, 200, 50 }, graphics::PathStyle().stoke(colors::Red));

    //graphics.drawImage(graphics::Image("dota2.jpg"), { 100, 100, 300, 200 });
    //graphics.drawImage(graphics::Image("dota2.jpg"), { 410, 100, 300, 200 });
    //graphics.drawImage(graphics::Image("1920.png"), rc32f{ 100, 100, 1280, 720 }, rc32i{ 0, 0, 1920, 1080 });
    //graphics.drawImage(graphics::Image("1920.png"), rc32f{ 100, 100, 1280, 720 }, rc32i{ 0, 540, 960, 540 });
    //graphics.drawImage(graphics::Image("1920.png"), { 100, 100 }, rc32i{ 500, 80, 960, 960 }, align::leftTop);

    graphics::TextBlob blob(u8"一二三四五六七八九十ABCD EFGHI𪚥𪚥𪚥ยิ้ยิ้تىلىشۆھرەت زاكىر ئۈرۈمچى شەھىرىدە تەكشۈرۈپ 123456 تەتقىق قىلدى", -1,
        graphics::StringFormat({ "", 20 }).color(colors::Black).align(align::rightBottom));
    counter_fps<float, 3> cps;
    while(true)
    {
        graphics.drawString(u8"一二三四五六七八九十ABCD EFGHI𪚥𪚥𪚥ยิ้ยิ้تىلىشۆھرەت زاكىر ئۈرۈمچى شەھىرىدە تەكشۈرۈپ 123456 تەتقىق قىلدى", rc32f(cx / 2, cy / 2, cx / 2, cy / 2),
            graphics::StringFormat({ "", 20 }).color(colors::Black).align(align::rightBottom));
        //graphics.drawString(u8"各位啊两个金额哇啦关雎尔挖掘各位甲骨文阿留个空位过了旺季安哥拉晋文公来围观", { 10, 400, 600, 600 }, { "", 40, 0 });
        graphics.drawTextBlob(blob, pt32f(cx / 2, cy / 2));
        cps.acc(1);
        printf("\r%.1f", cps.fps());
    }

    //graphics.drawString(u8"各位啊两个金额哇啦关雎尔挖掘各位甲骨文阿留个空位过了旺季安哥拉晋文公来围观", { 100, 100, 600, 600 }, { "", 40, 0 });
    cps.acc(1);
    printf("\r%.1f", cps.fps());

    //item.Draw(hdc2, drawX, drawY, { drawX, drawY, rc.right - frameSize * 2, rc.bottom - frameSize * 2 });

    //testAgg(pixmap);
    bitmap->Save("out.png");
    //image->Save("pd2.jpg");

    return 0;
}