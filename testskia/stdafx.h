#pragma once


#define NOMINMAX

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stdint.h>
#include <assert.h>

#include <cctype>
#include <locale>
#include <string>
#include <variant>
#include <any>

#include <array>
#include <vector>
#include <map>
#include <list>
#include <memory>
#include <map>
#include <queue>

#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>

#include <numeric>
#include <functional>
#include <algorithm> 
#include <regex>
#include <random>

#include <mutex>
#include <atomic>
#include <future>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;


#define USE_UTF8
typedef std::string a8string;
typedef std::string u8string;
typedef std::wstring u16string;
typedef char byte_t;

#include "common/error.h"
#include "common/system.h"
#include "common/logger.h"
#include "common/path.h"

#include <guiddef.h>

#include "resource.h"

inline double_t get_time_hr()
{
    auto now = std::chrono::high_resolution_clock::now();
    auto nowmcs = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    return nowmcs / 1000000.0;
}

inline double_t get_time_now_ms()
{
    return get_time_hr() * 1000;
}

//#if defined(_DEBUG)
//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
//#define debug_new new(_CLIENT_BLOCK, __FILE__,__LINE__)
//#define new debug_new
//#endif

#define _PLAYER_DEBUG 1


#define _VS2005_
#if _MSC_VER < 1400
#undef _VS2005_
#endif

#if !defined(SAFE_DELETE)
#define SAFE_DELETE(x) if(x) delete x; x=NULL;
#endif

#if !defined(SAFE_DELETE_ARRAY)
#define SAFE_DELETE_ARRAY(x) if (x) delete [] x; x=NULL; 
#endif

#if !defined(SAFE_RELEASE)
#define SAFE_RELEASE(x) if(x) x->Release(); x=NULL;
#endif

#ifdef UNICODE
#define _tcssprintf wsprintf
#define tcsplitpath _wsplitpath
#else
#define _tcssprintf sprintf
#define tcsplitpath _splitpath
#endif
