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
#include "graphics/raster/agg.h"
#include "core/io/filestream.h"

using namespace core;

agg::rendering_buffer rbuf;
agg::rasterizer_scanline_aa<> raster;
agg::scanline_u8 sl;


#if 1
void testAgg(std::shared_ptr<graphics::Pixmap> & pixmap)
{
    auto buffer = pixmap->buffer();
    rbuf.attach((agg::int8u *)buffer.data, buffer.size.cx, buffer.size.cy, buffer.flip_y ? -buffer.pitch : buffer.pitch);
    agg::pixel_accessor_bgra32 pixf(rbuf);
    agg::pixel_accessor_bgra32_pre pixf_pre(rbuf);
    agg::renderer_base<agg::pixel_accessor_bgra32> renb(pixf);
    agg::renderer_base<agg::pixel_accessor_bgra32_pre> ren_pre(pixf_pre);

    //renb.clear(agg::rgba(1, 1, 1));

    //agg::path_storage path;
    //path.remove_all();
    //path.move_to(28.47, 6.45);
    //path.curve3(21.58, 1.12, 19.82, 0.29);
    //path.curve3(17.19, -0.93, 14.21, -0.93);
    //path.curve3(9.57, -0.93, 6.57, 2.25);
    //path.curve3(3.56, 5.42, 3.56, 10.60);
    //path.curve3(3.56, 13.87, 5.03, 16.26);
    //path.curve3(7.03, 19.58, 11.99, 22.51);
    //path.curve3(16.94, 25.44, 28.47, 29.64);
    //path.line_to(28.47, 31.40);
    //path.curve3(28.47, 38.09, 26.34, 40.58);
    //path.curve3(24.22, 43.07, 20.17, 43.07);
    //path.curve3(17.09, 43.07, 15.28, 41.41);
    //path.curve3(13.43, 39.75, 13.43, 37.60);
    //path.line_to(13.53, 34.77);
    //path.curve3(13.53, 32.52, 12.38, 31.30);
    //path.curve3(11.23, 30.08, 9.38, 30.08);
    //path.curve3(7.57, 30.08, 6.42, 31.35);
    //path.curve3(5.27, 32.62, 5.27, 34.81);
    //path.curve3(5.27, 39.01, 9.57, 42.53);
    //path.curve3(13.87, 46.04, 21.63, 46.04);
    //path.curve3(27.59, 46.04, 31.40, 44.04);
    //path.curve3(34.28, 42.53, 35.64, 39.31);
    //path.curve3(36.52, 37.21, 36.52, 30.71);
    //path.line_to(36.52, 15.53);
    //path.curve3(36.52, 9.13, 36.77, 7.69);
    //path.curve3(37.01, 6.25, 37.57, 5.76);
    //path.curve3(38.13, 5.27, 38.87, 5.27);
    //path.curve3(39.65, 5.27, 40.23, 5.62);
    //path.curve3(41.26, 6.25, 44.19, 9.18);
    //path.line_to(44.19, 6.45);
    //path.curve3(38.72, -0.88, 33.74, -0.88);
    //path.curve3(31.35, -0.88, 29.93, 0.78);
    //path.curve3(28.52, 2.44, 28.47, 6.45);
    //path.close_polygon();

    //path.move_to(28.47, 9.62);
    //path.line_to(28.47, 26.66);
    //path.curve3(21.09, 23.73, 18.95, 22.51);
    //path.curve3(15.09, 20.36, 13.43, 18.02);
    //path.curve3(11.77, 15.67, 11.77, 12.89);
    //path.curve3(11.77, 9.38, 13.87, 7.06);
    //path.curve3(15.97, 4.74, 18.70, 4.74);
    //path.curve3(22.41, 4.74, 28.47, 9.62);
    //path.close_polygon();

    //double radius = 30;

    //agg::trans_affine shape_mtx;
    //shape_mtx *= agg::trans_affine_scaling(4.0);
    //shape_mtx *= agg::trans_affine_translation(radius, radius);
    //path.transform(shape_mtx);

    //agg::conv_curve<agg::path_storage> shape(path);

    //agg::rect_d shape_bounds;
    //agg::bounding_rect_single(shape, 0, &shape_bounds.x1, &shape_bounds.y1, &shape_bounds.x2, &shape_bounds.y2);
    //agg::rect_d shadow_bounds;
    //shadow_bounds.x1 = 0;
    //shadow_bounds.y1 = 0;
    //shadow_bounds.x2 = shape_bounds.x2 + radius;
    //shadow_bounds.y2 = shape_bounds.y2 + radius;

    //core::math::si64f_t size = { shadow_bounds.x2, shadow_bounds.y2 };


    ////agg::render_scanlines_aa_solid(raster, sl, renb, agg::rgba(0.2, 0.3, 0));

    //agg::pod_array<agg::int8u> m_gray8_buf;
    //agg::rendering_buffer      m_gray8_rbuf;

    //m_gray8_buf.resize(size.cx * size.cy);
    //m_gray8_rbuf.attach(m_gray8_buf.data(), size.cx, size.cy, size.cx);
    //agg::pixel_accessor_gray8 pixf_gray8(m_gray8_rbuf);
    //agg::renderer_base<agg::pixel_accessor_gray8> renb_gray8(pixf_gray8);

    //renb_gray8.clear(agg::gray8(0));

    //raster.clip_box(0, 0, size.cx, size.cy);
    //raster.add_path(shape);
    //agg::render_scanlines_aa_solid(raster, sl, renb_gray8, agg::gray8(255));

    //core::math::pt32_t pos = { 400, 200 };
    //agg::rendering_buffer rbuf2;
    //agg::pixel_accessor_gray8 pixf2(rbuf2);
    //if (pixf2.attach(pixf_gray8, int(shadow_bounds.x1), int(shadow_bounds.y1), int(shadow_bounds.x2), int(shadow_bounds.y2)))
    //{
    //    //agg::stack_blur<agg::rgba8, agg::stack_blur_calc_rgb<>> stack_blur;
    //    //stack_blur.blur(pixf2, agg::uround(radius));
    //    agg::stack_blur_gray8(pixf2, agg::uround(radius), agg::uround(radius));

    //    //renb.blend_from_color(pixf2,
    //    //    agg::rgba8(160, 255, 200),
    //    //    0,
    //    //    pos.x,
    //    //    pos.y);

    //    agg::rgba8 color = agg::rgba8(80, 128, 80);
    //    agg::rgba8 lut[256];
    //    for (int cnt = 0; cnt < 256; ++cnt)
    //    {
    //        lut[cnt] = color;
    //        lut[cnt].a = std::sqrt(cnt / 255.0) * color.a;
    //    }

    //    renb.blend_from_lut(pixf2,
    //        lut,
    //        0,
    //        pos.x,
    //        pos.y);
    //}

    //raster.clip_box(0, 0, pixmap->size().cx, pixmap->size().cy);

    ////agg::trans_affine shape_mtx2;
    ////shape_mtx2 *= agg::trans_affine_translation(pos.x, pos.y);
    ////path.transform(shape_mtx2);
    ////agg::conv_curve<agg::path_storage> shape2(path);
    ////raster.add_path(shape2);
    ////agg::render_scanlines_aa_solid(raster, sl, renb, agg::rgba(0.2, 0.6, 0.2, 0.5));

    //agg::trans_affine trans = agg::trans_affine_translation(pos.x, pos.y);

    //agg::conv_curve<agg::path_storage> shape22(path);

    //agg::conv_transform<decltype(shape22)> shape2(shape22, trans);
    //agg::conv_curve<decltype(shape2)> curve(shape2);
    //agg::conv_stroke<decltype(shape2)> stroke(shape2);

    //raster.reset();
    //raster.add_path(curve);
    //agg::render_scanlines_aa_solid(raster, sl, renb, agg::rgba(0.0, 0.0, 0.0, 1));

    //stroke.width(4.0);
    //raster.reset();
    //raster.add_path(stroke);
    //agg::render_scanlines_aa_solid(raster, sl, renb, agg::rgba(0.2, 0.6, 0.6, 0.8));


    agg::path_storage ps;
    //agg::conv_stroke<agg::path_storage> pg(ps);
    //pg.width(2.0);

    core::math::rc32_t rect{0, 0, 800, 600};
    ps.move_to(rect.x, rect.y);
    ps.line_to(rect.right(), rect.y);
    ps.line_to(rect.right(), rect.bottom());
    ps.line_to(rect.x, rect.bottom());
    ps.close_polygon();
    raster.reset();
    raster.add_path(ps);

    auto[fileData, fileSize] = core::io::readFullFile("lcw.tga");
    graphics::image::image_data_t img;
    graphics::image::image_create(fileData.get(), fileSize, img);

    agg::rendering_buffer img_buf;
    img_buf.attach((agg::int8u *)img.data, img.format.width, img.format.height, img.pitch);
    agg::pixel_accessor_bgra32 img_pixf(img_buf);
    //agg::image_accessor_clip<agg::pixel_accessor_bgra32> img_src(img_pixf, agg::rgba_pre(0, 0.4, 0, 0.5));
    //agg::render_scanlines_aa_solid(raster, sl, renb, agg::tools::rgba(colors::Black));

    agg::span_interpolator_linear<> interpolator( 
        agg::trans_affine_scaling(img.format.width / (double)rect.cx, img.format.height / (double)rect.cy) *
        agg::trans_affine_translation(rect.x, rect.y));

    agg::span_image_filter_rgba_bilinear_clip<agg::pixel_accessor_bgra32, agg::span_interpolator_linear<>> sg(img_pixf, agg::rgba_pre(0, 0, 0, 0.5), interpolator);
    //agg::span_image_filter_rgb_bilinear_clip<agg::pixel_accessor_bgr24, agg::span_interpolator_linear<>> sg(img_pixf, agg::rgba_pre(0, 0, 0, 0.5), interpolator);

    agg::span_allocator<agg::rgba8> sa;
    agg::render_scanlines_aa(raster, sl, renb, sa, sg);

    //agg::render_scanlines_aa_solid(raster, sl, renb, agg::rgba(0.6, 0, 0, 0.8));
}
#endif


void testImages()
{
    std::make_shared<graphics::Image>(u8"jpg/对比.jpg")->Save(core::string::format("out/jpg_", u8"对比", ".tga"));
    for(auto & path : core::io::allPaths("bmp"))
    {
        if(path.u8string().find(".bmp") != std::string::npos)
            std::make_shared<graphics::Image>(path.u8string())->Save(core::string::format("out/bmp_", path.filename()));
    }
    for (auto & path : core::io::allPaths("tga"))
    {
        if (path.u8string().find(".tga") != std::string::npos)
            std::make_shared<graphics::Image>(path.u8string())->Save(core::string::format("out/tga_", path.stem(), ".tga"));
    }
    for (auto & path : core::io::allPaths("png"))
    {
        if (path.u8string().find(".png") != std::string::npos)
            std::make_shared<graphics::Image>(path.u8string())->Save(core::string::format("out/png_", path.stem(), ".tga"));
    }
    for (auto & path : core::io::allPaths("jpg"))
    {
        if (path.u8string().find(".jpg") != std::string::npos)
            std::make_shared<graphics::Image>(path.u8string())->Save(core::string::format("out/jpg_", path.stem(), ".tga"));
    }
    for (auto & path : core::io::allPaths("dds"))
    {
        if (path.u8string().find(".dds") != std::string::npos)
            std::make_shared<graphics::Image>(path.u8string())->Save(core::string::format("out/dds_", path.stem(), ".bmp"));
    }
}

int main()
{
    win32::Win32App app;
    //testImages();

    int32_t cx = 1280;
    int32_t cy = 720;
    std::shared_ptr<graphics::Pixmap> pixmap = std::make_shared<graphics::Pixmap>(graphics::si32_t{ cx, cy });
    auto graphics = std::make_shared<graphics::Graphics>(pixmap);
    
    graphics->Clear(colors::LightGray);

    auto image = std::make_shared<graphics::Image>("lcw.tga");
    //graphics->DrawImage(*(image.get()), { cx / 4, cy / 4 }, core::math::align::leftTop);
    //graphics->DrawImage(*image, { 160, 10 }, core::math::align::leftTop);
    //graphics->DrawImage(*image, { 200, 20 }, core::math::align::leftTop);
    //graphics->DrawImage(*image, { 200, 20, 400, 100 });
    graphics->DrawImage(*image, math::rc32_t{100, 100, 500, 100}, core::math::rc32_t{ 100, 0, 194, 215});

    //graphics->DrawImage(*(image.get()), { 0, 0, cx / 4, cy / 4 }, core::math::align::bottomCenterX);
    //graphics->DrawImage(*(image.get()), { cx / 4, 0, cx / 4, cy / 4 });

    //graphics->DrawImage(*(image.get()), { 0, 0, cx / 2, cy / 4 }, core::math::align::bottomCenterX);

    //graphics->DrawImage(*(image.get()), { cx / 4, cy / 4 }, { 20, 300, 600, 200 }, core::math::align::leftTop);

    //graphics->DrawImage(*(image.get()), { cx / 4, cy / 4, cx / 2, cy / 2 }, { 20, 300, 600, 200 });
    //graphics->DrawLine({ cx / 2, cy }, { cx / 2, 0 },  colors::Yellow, 1);

    graphics->DrawLine({ 0, cy / 2 }, { cx, cy / 2 }, colors::Red, 1);
    graphics->DrawLine({ cx / 2, 0 }, { cx / 2, cy }, colors::Red, 1);
    graphics->DrawLine({ 0, 0 }, { cx, cy}, colors::Red, 1);
    graphics->DrawLine({ cx, 0 }, { 0, cy }, colors::Red, 1);
    graphics->DrawEllipse(math::rc32_t{ cx / 4, cy / 4, cx / 2, cy / 2 }, colors::Red, 2.0f);
    graphics->DrawEllipse(math::rc32_t{ cx / 4, cy / 4, cx / 2, cy / 2 }, colors::Red, 2.0f);

    //graphics->FillRect({ cx / 4, cy / 4, cx / 2, cy / 2 }, colors::Green);
    graphics->DrawRect(math::rc32_t{ cx / 4, cy / 4, cx / 2, cy / 2 }.expand(-1), colors::Red, 1);
    graphics->DrawRoundRect(math::rc32_t{ cx / 4, cy / 4, cx / 2, cy / 2 }, colors::White, 5, 50);

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

    //auto cm = std::make_shared<graphics::Image>("lcw.tga");
    //cm->Save("cm.bmp");
    //graphics->DrawImage(*cm.get(), { 10, 10 }, 0);
    //graphics->DrawImage(*cm.get(), { 50, 50 }, 0);
    graphics->DrawString(u8"各位啊两个金额哇啦关雎尔挖掘各位甲骨文阿留个空位过了旺季安哥拉晋文公来围观", { 10, 10, 300, 100 }, { "", 40, 0, graphics::text::font::cleartype }, colors::Black, core::math::align::leftTop);
    graphics->DrawString(u8"各位啊两个金额哇啦关雎尔挖掘各位甲骨文阿留个空位过了旺季安哥拉晋文公来围观", { 10, 50, 300, 100 }, { "", 40, 0, graphics::text::font::gray }, colors::Black, core::math::align::leftTop);
    auto si = graphics->MeasureString(u8"各位啊两个金额哇啦关雎尔挖掘各位甲骨文阿留个空\r\n位过了旺季安哥拉晋文公来围观", {"", 10 });

    graphics->DrawRect({ 100, 100, 200, 50 }, colors::Red, 2.0f);
    graphics->FillRect({ 100, 160, 200, 50 }, 0x80ff0000);
    graphics->DrawRect({ 100, 220, 200, 50 }, colors::Red, 2.0f);

    graphics->DrawImage(graphics::Image("png/png2.png"), { 100, 100 }, math::align::leftTop);

    //testAgg(pixmap);
    pixmap->Save("out.bmp");
    //image->Save("pd2.jpg");

    return 0;
}