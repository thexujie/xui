#pragma once

#define NOMINMAX

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
#include <stack>

#include <sstream>
#include <fstream>
#include <iostream>

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

#include <filesystem>

using namespace std::chrono_literals;

#define USE_UTF8
typedef std::string a8string;
typedef std::string u8string;
typedef std::wstring u16string;
typedef char byte_t;

#ifndef _WIN64
typedef int32_t intx_t;
typedef uint32_t uintx_t;
typedef int16_t intx_h;
typedef uint16_t uintx_h;
#else
#define BIT64
typedef int64_t intx_t;
typedef uint64_t uintx_t;
typedef int32_t intx_h;
typedef uint32_t uintx_h;
#endif

typedef char char8_t;
typedef float float32_t;
typedef double float64_t;

#include "core/error.h"
#include "core/system.h"
#include "core/logger.h"
#include "core/path.h"
#include "core/color32.h"
#include "core/object.h"
#include "core/invokable.h"
#include "core/counter_fps.h"
#include "core/event.h"

#include "unit.h"
#include "utils.h"
#include "align.h"
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "edge.h"

#include "float2.h"
#include "float3.h"
#include "float4.h"
#include "quat4.h"
#include "float3x2.h"
#include "float4x4.h"

using namespace core::unit_literals;
