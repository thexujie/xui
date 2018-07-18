#include "stdafx.h"


//#include <hb-ot.h>
#include <unicode/brkiter.h>
#include <unicode/locid.h>
#include <unicode/stringpiece.h>
#include <unicode/ubidi.h>
#include <unicode/uchriter.h>
#include <unicode/unistr.h>
#include <unicode/uscript.h>
#include "scrptrun.h"


void test_icu()
{
    wchar_t text[] = L"一二三四五六七八九十ABCD EFGHI𪚥𪚥𪚥ยิ้ยิ้تىلىشۆھرەت زاكىر ئۈرۈمچى شەھىرىدە تەكشۈرۈپ 123456 تەتقىق قىلدى";

    UBiDi* bidi = ubidi_open();
    UBiDiLevel bidiReq = UBIDI_DEFAULT_LTR;
    int stringLen = wcslen(text);
    if (bidi)
    {
        std::wcout.imbue(std::locale(std::locale(), "", LC_CTYPE));
        UErrorCode status = U_ZERO_ERROR;
        ubidi_setPara_62(bidi, (const UChar *)text, stringLen, bidiReq, NULL, &status);
        if (U_SUCCESS(status))
        {
            int paraDir = ubidi_getParaLevel_62(bidi);
            int32_t numRuns = ubidi_countRuns_62(bidi, &status);

            for (size_t i = 0; i < size_t(numRuns); ++i)
            {
                int32_t startRun = -1;
                int32_t lengthRun = -1;
                UBiDiDirection runDir = ubidi_getVisualRun_62(bidi, i, &startRun, &lengthRun);
                bool isRTL = (runDir == UBIDI_RTL);
                std::wstring_view wsv(text + startRun, lengthRun);
                wprintf(L"Processing Bidi Run = %d -- run-start = %d, run-len = %d, isRTL = %d\n", i, startRun, lengthRun, isRTL);

                ScriptRun scriptRun((const UChar *)text, startRun, lengthRun);
                while (scriptRun.next())
                {
                    int32_t     start = scriptRun.getScriptStart();
                    int32_t     end = scriptRun.getScriptEnd();
                    UScriptCode code = scriptRun.getScriptCode();

                    printf("Script '%s' from %d to %d.\n", uscript_getName(code), start, end);
                }
            }
        }
    }
}

