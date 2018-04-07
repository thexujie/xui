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

    int32_t cx = 512;
    int32_t cy = 512;
    auto pixmap = std::make_shared<graphics::Pixmap>(graphics::si32_t{ cx, cy });
    auto graphics = std::make_shared<graphics::Graphics>(pixmap);
    
    graphics->Clear(colors::LightGray);

    graphics->DrawLine({ 0, cy / 2 }, { cx, cy / 2 }, colors::Red, 1);
    graphics->DrawLine({ cx / 2, 0 }, { cx / 2, cy }, colors::Red, 1);

    graphics->FillRect({ cx / 4, cy / 4, cx / 2, cy / 2 }, colors::Green);
    graphics->DrawRect(math::rc32_t{ cx / 4, cy / 4, cx / 2, cy / 2 }.expand(-1), colors::Red, 1);

    graphics->DrawString(u8"MMMM", colors::Red, { "", 20 }, { cx / 2, cy / 2 }, core::math::align::centerXY);

    //graphics->DrawString(u8"LTLT", colors::Black, { "", 20 }, { cx / 2, cy / 2 }, core::math::align::leftTop);
    //graphics->DrawString(u8"RTRT", colors::Gray, { "", 20 }, { cx / 2, cy / 2 }, core::math::align::rightTop);
    //graphics->DrawString(u8"RBRB", colors::Green, { "", 20 }, { cx / 2, cy / 2 }, core::math::align::rightBottom);
    //graphics->DrawString(u8"LBLB", colors::Blue, { "", 20 }, { cx / 2, cy / 2 }, core::math::align::leftBottom);

    graphics->DrawString(u8"LTLT", colors::Black, { "", 20 }, { cx / 2, cy / 2 }, core::math::align::leftCenterY);
    graphics->DrawString(u8"RTRT", colors::Gray, { "", 20 }, { cx / 2, cy / 2 }, core::math::align::topCenterX);
    graphics->DrawString(u8"RBRB", colors::Green, { "", 20 }, { cx / 2, cy / 2 }, core::math::align::rightCenterY);
    graphics->DrawString(u8"LBLB", colors::Blue, { "", 20 }, { cx / 2, cy / 2 }, core::math::align::bottomCenterX);

    graphics->DrawString(u8"LTLT", colors::Red, { "", 20 }, { cx / 4, cy / 4, cx / 2, cy / 2 }, core::math::align::leftTop);
    graphics->DrawString(u8"RTRT", colors::Yellow, { "", 20 }, { cx / 4, cy / 4, cx / 2, cy / 2 }, core::math::align::rightTop);
    graphics->DrawString(u8"RBRB", colors::Green, { "", 20 }, { cx / 4, cy / 4, cx / 2, cy / 2 }, core::math::align::rightBottom);
    graphics->DrawString(u8"LBLB", colors::Blue, { "", 20 }, { cx / 4, cy / 4, cx / 2, cy / 2 }, core::math::align::leftBottom);

    graphics->DrawString(u8"LTLT", colors::Red, { "", 20 }, { cx / 4, cy / 4, cx / 2, cy / 2 }, core::math::align::leftCenterY);
    graphics->DrawString(u8"RTRT", colors::Yellow, { "", 20 }, { cx / 4, cy / 4, cx / 2, cy / 2 }, core::math::align::topCenterX);
    graphics->DrawString(u8"RBRB", colors::Green, { "", 20 }, { cx / 4, cy / 4, cx / 2, cy / 2 }, core::math::align::rightCenterY);
    graphics->DrawString(u8"LBLB", colors::Blue, { "", 20 }, { cx / 4, cy / 4, cx / 2, cy / 2 }, core::math::align::bottomCenterX);

    auto image = std::make_shared<graphics::Image>("pd.jpg");
    graphics->DrawImage(*(image.get()), { cx / 4, cy / 4 }, core::math::align::leftTop);
    graphics->DrawImage(*(image.get()), { 0, 0, cx / 4, cy / 4 }, core::math::align::rightTop);

    pixmap->Save("test.bmp");
    image->Save("pd2.jpg");

    return 0;
}