#include "stdafx.h"
#include "platform.h"
#include <shlobj.h>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

namespace platform
{
	pointer_t instance()
	{
		assert((HINSTANCE)& __ImageBase == GetModuleHandleW(NULL));
		//return ((HINSTANCE)&__ImageBase);
		return (pointer_t)GetModuleHandleW(NULL);
	}

	void endLoop(int32_t ret)
	{
		::PostQuitMessage(ret);
	}

	int runLoop()
	{
		MSG msg = {};
		while (msg.message != WM_QUIT)
		{
			DWORD dwWait = MsgWaitForMultipleObjectsEx(0, NULL, INFINITE, QS_ALLEVENTS, MWMO_ALERTABLE);
			switch (dwWait)
			{
			default:
				break;
			}

			while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					break;
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
		}
		return (int)msg.wParam;
	}

	uint32_t process_id()
	{
		return GetCurrentProcessId();
	}

	uint32_t thread_id()
	{
		return GetCurrentThreadId();
	}

	core::filesystem::path temp_path()
	{
		wchar_t szPath[MAX_PATH] = {};
		DWORD dwLength = ::GetTempPathW(MAX_PATH, szPath);
		return core::filesystem::path(core::wstr_u8str(std::wstring(szPath, dwLength)));
	}

	core::filesystem::path appdata_path()
	{
		wchar_t szPath[MAX_PATH] = {};
		BOOL bSucc = ::SHGetSpecialFolderPathW(NULL, szPath, CSIDL_APPDATA, FALSE);
		if (!bSucc)
			return temp_path();
		return core::filesystem::path(core::wstr_u8str(std::wstring(szPath, std::wcslen(szPath))));
	}

	core::filesystem::path process_path()
	{
		char temp[512] = {};
		int32_t nchars = GetModuleFileNameA(NULL, temp, 512);
		return core::filesystem::path(std::string(temp, nchars));
	}

	core::filesystem::path current_path()
	{
		wchar_t temp[512] = {};
		int32_t nchars = GetCurrentDirectoryW(512, temp);
		return core::filesystem::path(std::wstring(temp, nchars));
	}

	static void __thread_set_name(uint32_t thread_id, const char * name)
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
		info.thread_id = int(thread_id);
		info.flags = 0;
		__try
		{
			RaiseException(MS_VC_EXCEPTION_SET_THREAD_NAME, 0, sizeof(THREADNAME_INFO) / sizeof(ULONG_PTR), (ULONG_PTR *)& info);
		}
		__except (EXCEPTION_CONTINUE_EXECUTION) { }
	}
	
	void set_thread_name(uint32_t thread_id, std::string name)
	{
		if (!thread_id)
			thread_id = GetCurrentThreadId();

		__thread_set_name(thread_id, name.c_str());
	}

	void set_thread_priority(core::thread_priority priority)
	{
		switch (priority)
		{
		case core::thread_priority_idle: ::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_IDLE);
			break;
		case core::thread_priority_lowest: ::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
			break;
		case core::thread_priority_low: ::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
			break;
		case core::thread_priority_normal: ::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
			break;
		case core::thread_priority_high: ::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
			break;
		case core::thread_priority_highest: ::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
			break;
		case core::thread_priority_realtime: ::SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
			break;
		default: break;
		}
	}
}
