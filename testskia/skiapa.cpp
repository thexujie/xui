#include "stdafx.h"


#include "tools/base64.h"
#include "tools/string.h"
#include "tools/network/addr.h"
#include "tools/network/socket.h"
#include "tools/network/http.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <objbase.h>
#include "tools/invokable.h"
#include "tools/timer_counter.h"

#include <Shellapi.h>
#include "win32/dll.h"

#include "SkBitmap.h"
#include "SkImage.h"
#include "SkPaint.h"
#include "SkRect.h"
#include "SkImageEncoder.h"
#include "SkCodec.h"
#include "SkTypeface.h"
#include "SkCanvas.h"

//#pragma comment(lib, "../deps/skia/bin/x642/skia.dll.lib")
//#pragma comment(lib, "E:/github/google/skia/out/x64/skia.dll.lib")
#pragma comment(lib, "E:/github/google/skia/out/clang/skia.dll.lib")

bool decode_file(const char* filename, SkBitmap* bitmap, SkColorType colorType = kN32_SkColorType, bool requireUnpremul = false) 
{
    sk_sp<SkData> data(SkData::MakeFromFileName(filename));
    std::unique_ptr<SkCodec> codec = SkCodec::MakeFromData(data);
    if (!codec)
    {
        return false;
    }

    SkImageInfo info = codec->getInfo().makeColorType(colorType);
    if (requireUnpremul && kPremul_SkAlphaType == info.alphaType())
    {
        info = info.makeAlphaType(kUnpremul_SkAlphaType);
    }

    if (!bitmap->tryAllocPixels(info))
    {
        return false;
    }

    return SkCodec::kSuccess == codec->getPixels(info, bitmap->getPixels(), bitmap->rowBytes());
}

int app_main(int argc, const wchar_t * argv[])
{
    common::global_logger glog("log.plog", common::log_dbg);
    int a = 23;
    int b = a;
#if 1
    const int width = 1920;
    const int height = 1080;

    SkBitmap bitmap;
    //bitmap.installPixels
    SkImageInfo ii = SkImageInfo::Make(1920, 1080, kBGRA_8888_SkColorType, kPremul_SkAlphaType);
    bitmap.allocPixels(ii, ii.minRowBytes());


    SkPaint paint;
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setColor(0xff1f78b4);
    paint.setStrokeWidth(8);

    bitmap.allocPixels();
    SkCanvas canvas(bitmap);
    canvas.clear(0x00000000);// 背景为透明色

    SkBitmap bmp;
    SkBitmap test;
    decode_file("cm.png", &bmp);
    decode_file("test.png", &test);

    //canvas.drawBitmapRect(test, SkIRect{ 0, 0, test.width(), test.height() }, { 0, 0, (float)bitmap.width(), (float)bitmap.height() }, NULL);

    tools::timer_counter<double, 3> acc;
    int index = 1;
    while(++index)
    {
        //canvas.clear(0x80506070);
        canvas.drawBitmapRect(test, SkIRect{ 0, 0, test.width(), test.height() }, { 0, 0, (float)bitmap.width(), (float)bitmap.height() }, NULL);
        for(int row = 0; row < 10; ++row)
        {
            for(int col = 0; col < 10; ++col)
                canvas.drawBitmap(bmp, 10 + col * (bmp.width() + 10), 10 + row * (bmp.height() + 10));
        }
        acc.acc(1);
        printf("\rfps=%lf", acc.avg());
        if(index == 1000)
        {
            SkFILEWStream stream("out.png");
            SkEncodeImage(&stream, bitmap, SkEncodedImageFormat::kPNG, 100);
        }
    }

    {
        SkRect rc;
        rc.fLeft = 123;
        rc.fTop = 0;
        rc.fRight = 222;
        rc.fBottom = 50;
        canvas.drawOval(rc, paint);
    }

    {
        const char * text = "Hellow Skia";
        paint.setARGB(255, 255, 0, 0);
        paint.setTextSize(50);
        canvas.drawText(text, strlen(text), 500, 500, paint);
    }

    {
        paint.setColor(SK_ColorYELLOW);
        canvas.drawCircle(100, 100, 50, paint);
    }

    {
        SkPaint paint1, paint2, paint3;

        paint1.setTextSize(64.0f);
        paint1.setAntiAlias(true);
        paint1.setColor(0xff4281A4);
        paint1.setStyle(SkPaint::kFill_Style);

        paint2.setTextSize(64.f);
        paint2.setAntiAlias(true);
        paint2.setColor(0xff9CAFB7);
        paint2.setStyle(SkPaint::kStroke_Style);
        paint2.setStrokeWidth(SkIntToScalar(3));

        paint3.setTextSize(64.0f);
        paint3.setAntiAlias(true);
        paint3.setColor(0xffE6B89C);
        paint3.setTextScaleX(SkFloatToScalar(1.5f));

        const char text[] = "i love you";
        canvas.drawText(text, strlen(text), 200.0f, 64.0f, paint1);
        canvas.drawText(text, strlen(text), 200.0f, 144.0f, paint2);
        canvas.drawText(text, strlen(text), 200.0f, 224.0f, paint3);
    }
    SkFILEWStream stream("out.png");
    SkEncodeImage(&stream, bitmap, SkEncodedImageFormat::kPNG, 100);


    //win32::dll dllObs("obs.dll");
    //auto pfn_obs_startup = dllObs.get<decltype(obs_startup) *>("obs_startup");
    //pfn_obs_startup("zh-CN", R"(C:\Users\xujie\AppData\Roaming\obs-studio/plugin_config)", NULL);

    //win32::dll HuyaEffectPro(common::current_path().append("plugins\\HuyaEffectPro.dll").string());

    //auto pfn_obs_module_load = HuyaEffectPro.get<decltype(obs_module_load)*>("obs_module_load");
    //auto pfn_obs_module_unload = HuyaEffectPro.get<decltype(obs_module_unload)*>("obs_module_unload");
    //auto pfn_obs_module_post_load = HuyaEffectPro.get<decltype(obs_module_post_load)*>("obs_module_post_load");
    //auto pfn_obs_module_set_locale = HuyaEffectPro.get<decltype(obs_module_set_locale)*>("obs_module_set_locale");
    //auto pfn_obs_module_free_locale = HuyaEffectPro.get<decltype(obs_module_free_locale)*>("obs_module_free_locale");
    //auto pfn_obs_module_text = HuyaEffectPro.get<decltype(obs_module_text)*>("obs_module_text");
    //auto pfn_obs_module_get_string = HuyaEffectPro.get<decltype(obs_module_get_string)*>("obs_module_get_string");
    //auto pfn_obs_current_module = HuyaEffectPro.get<decltype(obs_current_module)*>("obs_current_module");
    //bool loadok = pfn_obs_module_load();


    ////auto _obs = obs;
    //auto pfn_obs_module_name = HuyaEffectPro.get<decltype(obs_module_name)*>("obs_module_name");
    //auto name = pfn_obs_module_name();
#endif
    tools::invokable_clear();
    common::logger::act() << __FUNCTION__" end" << std::endl;
    return 0;
}


int _tmain(int argc, const TCHAR * argv[])
{
#if defined _DEBUG
    _CrtMemState stateOld, stateNew, stateDiff;
    _CrtMemCheckpoint(&stateOld);
#endif
    app_main(argc, argv);
#if defined _DEBUG
    _CrtMemCheckpoint(&stateNew);
    if (_CrtMemDifference(&stateDiff, &stateOld, &stateNew))
        _CrtMemDumpAllObjectsSince(&stateDiff);
#endif
    return 0;
}

int WINAPI _tWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR lpCmdLine,
    _In_ int nShowCmd)
{
    //AllocConsole();
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    wmain(argc, const_cast<const wchar_t **>(argv));
    LocalFree(argv);
}
