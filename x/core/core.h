#pragma once

#define NOMINMAX

#include <stdio.h>
#include <tchar.h>
#include <stdint.h>
#include <assert.h>

#include <cctype>
#include <cwctype>
#include <locale>
#include <string>
#include <variant>
#include <any>
#include <set>

#include <array>
#include <vector>
#include <map>
#include <unordered_map>
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

namespace core
{
    using char8_t = ::char8_t;

    using byte_t = ::byte_t;

    using int8_t = ::int8_t;
    using int16_t = ::int16_t;
    using int32_t = ::int32_t;
    using int64_t = ::int64_t;

    using uint8_t = ::uint8_t;
    using uint16_t = ::uint16_t;
    using uint32_t = ::uint32_t;
    using uint64_t = ::uint64_t;

    using float32_t = ::float32_t;
    using float64_t = ::float64_t;

    struct section
    {
        size_t index = 0;
        size_t length = 0;

        size_t end() const { return index + length; }
        section operator + (const section & rhs)
        {
            if (!length)
                return rhs;

            if (!rhs.length)
                return *this;

            if (index + length == rhs.index)
                return { index, rhs.index + rhs.length };

            if (rhs.index + rhs.length == index)
                return { rhs.index, index + length };

            return { 0, 0 };
        }

        section & operator += (const section & rhs)
        {
            if (!length)
                *this = rhs;
            else if (!rhs.length)
                ;
            else if (index + length == rhs.index)
                *this = { index, length + rhs.length };
            else if (rhs.index + rhs.length == index)
                *this = { rhs.index, length + rhs.length };
            else
                *this = { 0, 0 };
            return *this;
        }
    };
}

#include "core/member_function_traits.h"
#include "core/bitflag.h"
#include "core/error.h"
#include "core/system.h"
#include "core/unicode.h"
#include "core/logger.h"
#include "core/path.h"
#include "core/color32.h"
#include "core/object.h"
#include "core/counter_fps.h"
#include "core/event.h"
#include "core/unit.h"
#include "core/utils.h"
#include "core/datetime.h"

#include "core/align.h"
#include "core/vec2.h"
#include "core/vec3.h"
#include "core/vec4.h"
#include "core/edge.h"

#include "core/float2.h"
#include "core/float3.h"
#include "core/float4.h"
#include "core/quat4.h"
#include "core/float3x2.h"
#include "core/float4x4.h"

#include "core/attribute.h"
#include "core/property.h"

#include "core/timer.h"
#include "core/app.h"
#include "core/block_ptr.h"

#include "core/io/stream.h"
#include "core/io/filestream.h"

using namespace core::unit_literals;
