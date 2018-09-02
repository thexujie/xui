#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <objbase.h>

#include <Shellapi.h>

#include "SkBitmap.h"
#include "SkImage.h"
#include "SkPaint.h"
#include "SkRect.h"
#include "SkImageEncoder.h"
#include "SkCodec.h"
#include "SkTypeface.h"
#include "SkCanvas.h"
#include "SkRSXform.h"
#include "SkTextBlob.h"
#include "../x/core/core.h"
#include "SkGraphics.h"
#include "SkShaper.h"
#include "SkImage.h"
#include <hb-ot.h>
#include <unicode/brkiter.h>
#include <unicode/locid.h>
#include <unicode/stringpiece.h>
#include <unicode/ubidi.h>
#include <unicode/uchriter.h>
#include <unicode/unistr.h>
#include <unicode/uscript.h>

#include "SkFontMgr.h"
#include "SkOnce.h"
#include "SkShaper.h"
#include "SkStream.h"
#include "skia/src/core/SkTDPQueue.h"
#include "SkTLazy.h"
#include "SkTemplates.h"
#include "SkTextBlob.h"
#include "SkTo.h"
#include "SkTypeface.h"
#include "script.h"
//#include "../x/core/counter_fps.h"
//#include "../x/core/invokable.h"
//#include "../x/core/logger.h"

//#pragma comment(lib, "../deps/skia/bin/x642/skia.dll.lib")
//#pragma comment(lib, "E:/github/google/skia/out/x64/skia.dll.lib")
//#pragma comment(lib, "../externals/skia/bin/x64/skia.dll.lib")
#pragma comment(lib, "E:/github/google/skia/out/x64d/skia.dll.lib")

bool decode_file(const char* filename, SkBitmap* bitmap, SkColorType colorType = kN32_SkColorType, bool requireUnpremul = false) 
{
    sk_sp<SkData> data(SkData::MakeFromFileName(filename));
    SkImage::MakeFromEncoded(data);
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


void test_icu();
int app_main(int argc, const wchar_t * argv[])
{
#if 1
    test_icu();

    const int width = 1920;
    const int height = 1080;

    SkBitmap bitmap;
    //bitmap.installPixels
    //bitmap.allocPixels(SkImageInfo::Make(1920, 1080, kBGRA_8888_SkColorType, kPremul_SkAlphaType, nullptr), 0);


    SkBitmap bmp;
    SkBitmap test;
    decode_file("cm.png", &bmp);
    decode_file("test.png", &test);
    //canvas.drawBitmapRect(test, SkIRect{ 0, 0, test.width(), test.height() }, { 0, 0, (float)bitmap.width(), (float)bitmap.height() }, NULL);

    //bitmap.allocPixels();
    SkCanvas canvas(bitmap);
    canvas.clear(0x00000000);// 背景为透明色


	SkPaint paint00;
	paint00.setColor(0xffff0000);

    core::counter_fps<double, 3> acc;
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
        printf("\rfps=%lf", acc.fps());
        SkPaint paint0;
        paint0.setColor(0xF0FFFFFF);
        canvas.drawRect({0, 0, 1280, 720}, paint0);
        {
            SkPaint paint1, paint2, paint3;

            paint1.setTextSize(12.0f);
            paint1.setTypeface(SkTypeface::MakeFromName("微软雅黑", SkFontStyle::Normal()));
            paint1.setSubpixelText(true);
            paint1.setAntiAlias(true);
            paint1.setColor(0xff000000);
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
            std::string str = u8"一二三四五六七八九十ABCD EFGHI𪚥𪚥𪚥ยิ้ยิ้تىلىشۆھرەت زاكىر ئۈرۈمچى شەھىرىدە تەكشۈرۈپ 123456 تەتقىق قىلدى";
            //canvas.drawText(str.c_str(), str.length(), 200.0f, 64.0f, paint1);
            //canvas.drawText(text, strlen(text), 200.0f, 144.0f, paint2);
            //canvas.drawText(text, strlen(text), 200.0f, 224.0f, paint3);

            {
                static const char gText[] =
                    "When in the Course of human events it becomes necessary for one people "
                    "to dissolve the political bands which have connected them with another "
                    "and to assume among the powers of the earth, the separate and equal "
                    "station to which the Laws of Nature and of Nature's God entitle them, "
                    "a decent respect to the opinions of mankind requires that they should "
                    "declare the causes which impel them to the separation.";

				//SkGlyphRunBuilder gb;

                //SkTextBlobBuilder builder;
                //const SkTextBlobBuilder::RunBuffer & buffer = builder.allocRunText(
                //    paint1, 1, 0, 0, strlen(gText), SkString(gText), nullptr);

				canvas.drawLine(SkPoint{ 0.0f, 0.0f }, SkPoint{ (float)600, 0.0f }, paint00);

                //SkTextBlob tb;
                SkShaper shaper(nullptr);

                SkScalar margin = 20;

                SkPaint paint;
                paint.setAntiAlias(true);
                paint.setLCDRenderText(true);
                paint.setColor(0xff000000);

                core::counter_fps<float, 3> cpf(1s);
				int w = 500;
                while(true)
                {
                    for (int i = 20; i < 24; i += 2)
                    {
                        sk_sp<SkTextBlob> blob;
                        {
                            SkTextBlobBuilder builder;
                            paint.setTextSize(SkIntToScalar(i));
                            SkRect sr;
                            SkPoint end = shaper.shape(builder, paint, str.c_str(), str.length(), true, {}, w, sr);
                            blob = builder.make();
                        }
                        //canvas.drawLine(SkPoint{ 0.0f, 0.0f }, SkPoint{(float) w, 0.0f }, paint);
                        //SkPoint end = shaper.shape(&builder, paint, str.c_str(), str.length(), true, { margin, margin }, w - margin);
                        //SkPoint end = shaper.shape(&builder, paint, gText, strlen(gText), true, { margin, margin }, w - margin);
                        //canvas.drawRect({ 0, 0, 500, end.y() }, paint00);
                        //paint00.setColor((i * 0x11) | 0xff000000);
                        while (true)
                        {
                            canvas.drawTextBlob(blob, 0, 0, paint);
                            //canvas.translate(0, end.y());
                            cpf.acc(1);
                            printf("\rfps=%f", cpf.fps());
                        }
                    }
                }
            }
            //{
            //    const int iterations = 26;
            //    SkRSXform transforms[iterations];
            //    char alphabet[iterations];
            //    SkScalar angle = 0;
            //    SkScalar scale = 1;
            //    for (size_t i = 0; i < SK_ARRAY_COUNT(transforms); ++i)
            //    {
            //        const SkScalar s = SkScalarSin(angle) * scale;
            //        const SkScalar c = SkScalarCos(angle) * scale;
            //        transforms[i] = SkRSXform::Make(-c, -s, -s * 16, c * 16);
            //        angle += .45;
            //        scale += .2;
            //        alphabet[i] = 'A' + i;
            //    }
            //    SkPaint paint;
            //    paint.setTextAlign(SkPaint::kCenter_Align);
            //    canvas.translate(110, 138);
            //    canvas.drawTextRSXform(alphabet, sizeof(alphabet), transforms, nullptr, paint1);
            //}
        }

        //if(index == 1000)
        {
            SkFILEWStream stream("out.png");
            SkEncodeImage(&stream, bitmap, SkEncodedImageFormat::kPNG, 100);
        }

        return 0;
    }



    SkPaint paint;
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setColor(0xff1f78b4);
    paint.setStrokeWidth(8);

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
    core::logger::act() << __FUNCTION__" end" << std::endl;
    return 0;
}

class DumpT
{
public:
	int val = 0;
	DumpT(int val_) : val(val_)
	{
		
	}
	~DumpT()
	{
		std::cout << "fun1()  executed!" << std::endl;
	}
};

static std::unique_ptr<DumpT> dummpt;


// Main function which prints itself just so we can see where the app actually enters
int _tmain(int argc, const TCHAR * argv[])
{
	dummpt = std::make_unique<DumpT>(1);
#if defined _DEBUG
    _CrtMemState stateOld, stateNew, stateDiff;
    _CrtMemCheckpoint(&stateOld);
#endif

    std::string str = u8"abcd我爱㌶㍍㌶㌫㍊㍍我家𪚥𪚥𪚥 hello world تەتقىق قىلدىhello world تەتقىق قىلدى";
    script::Shaper shaper;
    shaper.reset(str);

    //UErrorCode status = U_ZERO_ERROR;
    //std::unique_ptr<UBiDi, decltype(&ubidi_close)> bidi(ubidi_openSized(str.length(), 0, &status), &ubidi_close);
    //if (U_FAILURE(status))
    //{
    //    SkDebugf("Bidi error: %s", u_errorName(status));
    //}
    //SkASSERT(bidi);

    //// The required lifetime of utf16 isn't well documented.
    //// It appears it isn't used after ubidi_setPara except through ubidi_getText.
    //ubidi_setPara(bidi.get(), str.c_str(), str.length(), UBIDI_DEFAULT_LTR, nullptr, &status);
    //if (U_FAILURE(status))
    //{
    //    SkDebugf("Bidi error: %s", u_errorName(status));
    //}

    //auto bi = icu::BreakIterator::createLineInstance(icu::Locale::getDefault(), status);
    //std::u16string str3 = u"abcd我爱我家𪚥𪚥𪚥 hello world";
    //UText text = UTEXT_INITIALIZER;
    //utext_openUChars(&text, str3.data(), str3.length(), &status);
    //bi->setText(&text, status);

    //char32_t c = U'𪚥';
    //std::vector<int> bi1;
    //std::vector<int> bi2;

    //int32_t breakIteratorCurrent = bi->current();
    //while (breakIteratorCurrent != icu::BreakIterator::DONE)
    //{
    //    bi1.push_back(breakIteratorCurrent);
    //    breakIteratorCurrent = bi->next();
    //}

    //utext_openUTF8(&text, str2.data(), str2.length(), &status);
    //bi->setText(&text, status);
    //int32_t breakIteratorCurrent2 = bi->current();
    //while (breakIteratorCurrent2 != icu::BreakIterator::DONE)
    //{
    //    bi2.push_back(breakIteratorCurrent2);
    //    breakIteratorCurrent2 = bi->next();
    //}

	//_CrtSetBreakAlloc(758);
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
