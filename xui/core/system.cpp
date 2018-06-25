#include "stdafx.h"
#include "system.h"
#include "path.h"
#include "string.h"
#include "win32/windows.h"
#include <shlobj.h>

namespace core
{
    core::filesystem::path temp_path()
    {
        wchar_t szPath[MAX_PATH] = {};
        DWORD dwLength = ::GetTempPathW(MAX_PATH, szPath);
        return core::filesystem::path(core::string::u16_u8(std::wstring(szPath, dwLength)));
    }

    core::filesystem::path appdata_path()
    {
        wchar_t szPath[MAX_PATH] = {};
        BOOL bSucc = ::SHGetSpecialFolderPathW(NULL, szPath, CSIDL_APPDATA, FALSE);
        if (!bSucc)
            return temp_path();
        return core::filesystem::path(core::string::u16_u8(std::wstring(szPath, std::wcslen(szPath))));
    }

    core::filesystem::path process_path()
    {
        char temp[512] = {};
        int32_t nchars = GetModuleFileNameA(NULL, temp, 512);
        return core::filesystem::path(std::string(temp, nchars));
    }

    std::string process_name()
    {
        return process_path().filename().string();
    }

    int32_t thread_id()
    {
        return ::GetCurrentThreadId();
    }

    int32_t process_id()
    {
        return ::GetCurrentProcessId();
    }

    static void __thread_set_name(int thread_id, const char * name)
    {
        if (!thread_id)
            thread_id = GetCurrentThreadId();

#pragma pack(push, 8)
        struct THREADNAME_INFO
        {
            uint32_t dwType; // must be 0x1000
            const char * name; // pointer to name (in same addr space)
            int thread_id; // thread ID (-1 caller thread)
            uint32_t flags; // reserved for future use, most be zero
        };
#pragma pack(pop)

        const uint32_t MS_VC_EXCEPTION_SET_THREAD_NAME = 0x406d1388;
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.name = name;
        info.thread_id = thread_id;
        info.flags = 0;
        __try
        {
            RaiseException(MS_VC_EXCEPTION_SET_THREAD_NAME, 0, sizeof(THREADNAME_INFO) / sizeof(ULONG_PTR), (ULONG_PTR *)&info);
        }
        __except (EXCEPTION_CONTINUE_EXECUTION) { }
    }

    void thread_set_name(int thread_id, std::string name)
    {
        if (!thread_id)
            thread_id = GetCurrentThreadId();

        std::string namea = core::string::u8_ansi(name);
        __thread_set_name(thread_id, namea.c_str());
    }

    void thread_set_priority(thread_priority priority)
    {
        switch (priority)
        {
        case thread_priority_idle: ::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_IDLE);
            break;
        case thread_priority_lowest: ::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
            break;
        case thread_priority_low: ::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
            break;
        case thread_priority_normal: ::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
            break;
        case thread_priority_high: ::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
            break;
        case thread_priority_highest: ::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
            break;
        case thread_priority_realtime: ::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
            break;
        default: break;
        }
    }
}
