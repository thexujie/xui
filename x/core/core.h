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
#include <bitset>
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
#include <optional>

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

typedef float float32_t;
typedef double float64_t;

using pointer_t = void *;

namespace core
{

    template<typename T>
    struct span_type
    {
        T index = 0;
        T length = 0;

        T end() const { return index + length; }
        span_type operator + (const span_type & rhs)
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

        span_type & operator += (const span_type & rhs)
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

    typedef span_type<size_t> span;
    typedef span_type<uint32_t> span32;
    typedef span_type<uint16_t> span16;
    typedef span_type<uint8_t> span8;
}

#ifndef __cpp_char8_t
using char8_t = char;
namespace std
{
	using u8string = std::string;
	using u8string_view = std::string_view;
}
#endif


#include "core/member_traits.h"
#include "core/bitflag.h"
#include "core/error.h"
#include "core/system.h"
#include "core/logger.h"
#include "core/path.h"
#include "core/color.h"
#include "core/object.h"
#include "core/counter_fps.h"
#include "core/event.h"
#include "core/unit.h"
#include "core/utils.h"
#include "core/datetime.h"
#include "core/easingcurve.h"
#include "core/format.h"

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

#include "core/comptr.h"

using namespace core::unit_literals;

namespace core
{
    typedef core::vec2<core::dimenf> dimen2f;
    typedef core::vec4<core::dimenf> dimen4f;
}
