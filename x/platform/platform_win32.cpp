#include "stdafx.h"
#include "platform_win32.h"
#include "win32/win32.h"
#include <shlobj.h>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

namespace core
{
	using namespace win32;
	
	pointer_t instance()
	{
		assert((HINSTANCE)& __ImageBase == GetModuleHandleW(NULL));
		//return ((HINSTANCE)&__ImageBase);
		return (pointer_t)GetModuleHandleW(NULL);
	}

	void quit(int32_t ret)
	{
		::PostQuitMessage(ret);
	}

	int32_t loop()
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
		return (int32_t)msg.wParam;
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


	std::u8string winerr_str(int32_t err)
	{
		wchar_t buffer[512] = {};
		int nchars = ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, 512, NULL);
		if (!nchars)
			return core::fmt(u8"error ", err);

		std::wstring str(buffer, nchars - 2);
		return core::wstr_u8str(str);
	}


	std::u8string GUID2String(const GUID & guid)
	{
		std::u8string output(40, 0);
		snprintf(reinterpret_cast<char *>(output.data()), output.size(), "{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}",
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6],
			guid.Data4[7]);
		return output;
	}

	std::u8string GUID2String(std::array<uint8_t, 16> data)
	{
		const GUID & guid = *(const GUID *)data.data();
		std::u8string output(40, 0);
		snprintf(reinterpret_cast<char *>(output.data()), output.size(), "{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}",
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6],
			guid.Data4[7]);
		return output;
	}


	std::tuple<std::shared_ptr<byte_t>, int32_t> load_res(uint32_t id, res_e type)
	{
		static const char RES_TYPES[res_count][16] = { "FILE" };

		int index = std::clamp((int)type, 0, res_count - 1);
		HRSRC hRes = FindResourceA(NULL, (const char *)(uintmax_t)id, RES_TYPES[index]);
		if (hRes)
		{
			HGLOBAL hData = LoadResource(NULL, hRes);
			if (hData)
			{
				byte_t * ptr = (byte_t *)LockResource(hData);
				uint32_t nbRes = SizeofResource(NULL, hRes);

				std::shared_ptr<byte_t> data(ptr, [hData](byte_t * ptr)
					{
						UnlockResource(hData);
						FreeResource(hData);
					});
				return { data, (int32_t)nbRes };
			}
		}
		return {};
	}

	winversion_t version()
	{
		static winversion_t version = {};
		if (version.major == 0)
		{
			typedef LONG(WINAPI * RtlGetVersionFuncT)(RTL_OSVERSIONINFOEXW *);
			typedef void (WINAPI * NtVersionNumbersFuncT)(DWORD *, DWORD *, DWORD *);

			RTL_OSVERSIONINFOEXW rtlOSVersion = { sizeof(RTL_OSVERSIONINFOEXW) };
			RtlGetVersionFuncT pfnRtlGetVersion = (RtlGetVersionFuncT)(::GetProcAddress(::GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion"));
			if (pfnRtlGetVersion)
			{
				pfnRtlGetVersion(&rtlOSVersion);
			}

			if (rtlOSVersion.dwMajorVersion > 5)
			{
				version.major = rtlOSVersion.dwMajorVersion;
				version.minor = rtlOSVersion.dwMinorVersion;
				version.pack = rtlOSVersion.wServicePackMajor;
				version.build = rtlOSVersion.dwBuildNumber;
			}
			else
			{
				NtVersionNumbersFuncT pfnRtlGetNtVersionNumbers = (NtVersionNumbersFuncT)(::GetProcAddress(::GetModuleHandleW(L"ntdll.dll"), "RtlGetNtVersionNumbers"));
				if (pfnRtlGetNtVersionNumbers)
				{
					DWORD dwMajor = 0;
					DWORD dwMinor = 0;
					DWORD dwBuild = 0;
					pfnRtlGetNtVersionNumbers(&dwMajor, &dwMinor, &dwBuild);
					dwBuild &= 0xffff;
					version.major = dwMajor;
					version.minor = dwMinor;
					version.pack = 0;
					version.build = dwBuild;
				}
				else
				{
					version.major = rtlOSVersion.dwMajorVersion;
					version.minor = rtlOSVersion.dwMinorVersion;
					version.pack = rtlOSVersion.dwMajorVersion;
					version.build = rtlOSVersion.dwBuildNumber;
				}
			}
		}

		return version;
	}

	std::u8string version_str(const winversion_t & ver)
	{
		if (ver >= winversion_10)
			return u8"10";
		else if (ver >= winversion_8_1)
			return u8"8.1";
		if (ver >= winversion_8)
			return u8"8";
		if (ver >= winversion_7)
			return u8"7";
		if (ver >= winversion_vista)
			return u8"Vista";
		if (ver >= winversion_server_2003)
			return u8"Server 2003";
		if (ver >= winversion_xp_sp3)
			return u8"XP SP3";
		if (ver >= winversion_xp_sp2)
			return u8"XP SP2";
		if (ver >= winversion_xp_sp1)
			return u8"XP SP1";
		if (ver >= winversion_xp)
			return u8"XP";
		if (ver >= winversion_2000)
			return u8"2000";
		return u8"Unknown";
	}

	void debug_output(const std::u8string & str)
	{
		::OutputDebugStringW(u8str_wstr(str).c_str());
	}
}
