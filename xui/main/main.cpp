#include "stdafx.h"

#include "core/app.h"
#include "core/math/vec2.h"
#include "graphics/GraphicsService.h"
#include "graphics/Graphics.h"
#include "win32/Bitmap.h"
#include "win32/windows.h"
#include "win32/Graphics.h"
#include "win32/GraphicsService.h"
#include "win32/Win32App.h"
#include "graphics/Image.h"

using namespace core;

#pragma comment(lib, "Msimg32.lib")

int main()
{
    win32::Win32App app;

    int32_t cx = 1280;
    int32_t cy = 720;
    auto pixmap = std::make_shared<graphics::Pixmap>(graphics::si32_t{ cx, cy });
    auto graphics = std::make_shared<graphics::Graphics>(pixmap);
    
    graphics->Clear(colors::LightGray);

    auto image = std::make_shared<graphics::Image>("dota2.jpg");
    //graphics->DrawImage(*(image.get()), { cx / 4, cy / 4 }, core::math::align::leftTop);
    //graphics->DrawImage(*(image.get()), { 0, 0, cx / 4, cy / 4 }, core::math::align::bottomCenterX);
    //graphics->DrawImage(*(image.get()), { cx / 4, 0, cx / 4, cy / 4 });

    //graphics->DrawImage(*(image.get()), { 0, 0, cx / 2, cy / 4 }, core::math::align::bottomCenterX);

    //graphics->DrawImage(*(image.get()), { cx / 4, cy / 4 }, { 20, 300, 600, 200 }, core::math::align::leftTop);

    graphics->DrawImage(*(image.get()), { cx / 4, cy / 4, cx / 2, cy / 2 }, { 20, 300, 600, 200 });
    //graphics->DrawLine({ cx / 2, cy }, { cx / 2, 0 },  colors::Yellow, 1);

    graphics->DrawLine({ 0, cy / 2 }, { cx, cy / 2 }, colors::Red, 1);
    graphics->DrawLine({ cx / 2, 0 }, { cx / 2, cy }, colors::Red, 1);

    //graphics->FillRect({ cx / 4, cy / 4, cx / 2, cy / 2 }, colors::Green);
    graphics->DrawRect(math::rc32_t{ cx / 4, cy / 4, cx / 2, cy / 2 }.expand(-1), colors::Red, 1);

    graphics->DrawString(u8"MMMM", { cx / 2, cy / 2 }, { "", 10 }, colors::Red, core::math::align::centerXY);

    //graphics->DrawString(u8"LTLT", colors::Black, { "", 10 }, { cx / 2, cy / 2 }, core::math::align::leftTop);
    //graphics->DrawString(u8"RTRT", colors::Gray, { "", 10 }, { cx / 2, cy / 2 }, core::math::align::rightTop);
    //graphics->DrawString(u8"RBRB", colors::Green, { "", 10 }, { cx / 2, cy / 2 }, core::math::align::rightBottom);
    //graphics->DrawString(u8"LBLB", colors::Blue, { "", 10 }, { cx / 2, cy / 2 }, core::math::align::leftBottom);

    graphics->DrawString(u8"LTLT", { cx / 2, cy / 2 }, { "", 10 }, colors::Black, core::math::align::leftCenterY);
    graphics->DrawString(u8"RTRT", { cx / 2, cy / 2 }, { "", 10 }, colors::Gray, core::math::align::topCenterX);
    graphics->DrawString(u8"RBRB", { cx / 2, cy / 2 }, { "", 10 }, colors::Green, core::math::align::rightCenterY);
    graphics->DrawString(u8"LBLB", { cx / 2, cy / 2 }, { "", 10 }, colors::Blue, core::math::align::bottomCenterX);

    graphics->DrawString(u8"LTLT", { cx / 4, cy / 4, cx / 2, cy / 2 }, { "", 10 }, colors::Red, core::math::align::leftTop);
    graphics->DrawString(u8"RTRT", { cx / 4, cy / 4, cx / 2, cy / 2 }, { "", 10 }, colors::Yellow, core::math::align::rightTop);
    graphics->DrawString(u8"RBRB", { cx / 4, cy / 4, cx / 2, cy / 2 }, { "", 10 }, colors::Green, core::math::align::rightBottom);
    graphics->DrawString(u8"LBLB", { cx / 4, cy / 4, cx / 2, cy / 2 }, { "", 10 }, colors::Blue, core::math::align::leftBottom);

    graphics->DrawString(u8"LTLT", { cx / 4, cy / 4, cx / 2, cy / 2 }, { "", 10 }, colors::Red, core::math::align::leftCenterY);
    graphics->DrawString(u8"RTRT", { cx / 4, cy / 4, cx / 2, cy / 2 }, { "", 10 }, colors::Yellow, core::math::align::topCenterX);
    graphics->DrawString(u8"RBRB", { cx / 4, cy / 4, cx / 2, cy / 2 }, { "", 10 }, colors::Green, core::math::align::rightCenterY);
    graphics->DrawString(u8"LBLB", { cx / 4, cy / 4, cx / 2, cy / 2 }, { "", 10 }, colors::Blue, core::math::align::bottomCenterX);

    auto cm = std::make_shared<graphics::Image>("lcw.tga");
    cm->Save("cm.bmp");
    graphics->DrawImage(*cm.get(), { 10, 10 }, 0);
    graphics->DrawImage(*cm.get(), { 50, 50 }, 0);
    graphics->DrawString(u8"各位啊两个金额哇啦关雎尔挖掘各位甲骨文阿留个空位过了旺季安哥拉晋文公来围观", { 10, 10, 68, 300 }, { "", 10 }, colors::Black, core::math::align::leftTop);
    auto si = graphics->MeasureString(u8"各位啊两个金额哇啦关雎尔挖掘各位甲骨文阿留个空\r\n位过了旺季安哥拉晋文公来围观", {"", 10 });

    pixmap->Save("test.bmp");
    //image->Save("pd2.jpg");

    return 0;
}