#include "stdafx.h"
#include "property.h"

namespace core
{
    static inline int32_t _number(char ch)
    {
        if ('a' <= ch && ch <= 'z')
            return 10 + (ch - 'a');
        if ('A' <= ch && ch <= 'A')
            return 10 + (ch - 'A');
        if ('0' <= ch && ch <= '9')
            return ch - '0';
        return 0;
    }

    std::string parseString(const std::string & str)
    {
        return std::string(str.data(), str.length());
    }

    core::color32 parseColor(const std::string & str)
    {
        if (str.empty())
            return colors::Auto;

        // #AABBCCDD
        if (str[0] == '#')
        {
            // #FFAAAAAA
            if (str.length() == 2)
            {
                int32_t a = _number(str[1]);
                int32_t aa = a | a << 4;
                return 0xff000000 | (aa << 16) | (aa << 8) | aa;
            }

            // #FFAABBCC
            if (str.length() == 4)
            {
                int32_t a = _number(str[1]);
                int32_t b = _number(str[2]);
                int32_t c = _number(str[3]);
                int32_t aa = a | a << 4;
                int32_t bb = b | b << 4;
                int32_t cc = c | c << 4;
                return 0xff000000 | (aa << 16) | (bb << 8) | cc;
            }

            // #AABBCCDD
            if (str.length() == 5)
            {
                int32_t a = _number(str[1]);
                int32_t b = _number(str[2]);
                int32_t c = _number(str[3]);
                int32_t d = _number(str[4]);
                int32_t aa = a | a << 4;
                int32_t bb = b | b << 4;
                int32_t cc = c | c << 4;
                int32_t dd = d | d << 4;
                return (aa << 24) | (bb << 16) | (cc << 8) | dd;
            }

            // #FFABCDEF
            if (str.length() == 7)
            {
                int32_t a = _number(str[1]);
                int32_t b = _number(str[2]);
                int32_t c = _number(str[3]);
                int32_t d = _number(str[4]);
                int32_t e = _number(str[5]);
                int32_t f = _number(str[6]);
                return 0xff000000 | (a << 20) | (b << 16) | (c << 12) | (d << 8) | (e << 4) | (f << 0);
            }

            // #ABCDEFGH
            if (str.length() == 9)
            {
                int32_t a = _number(str[1]);
                int32_t b = _number(str[2]);
                int32_t c = _number(str[3]);
                int32_t d = _number(str[4]);
                int32_t e = _number(str[5]);
                int32_t f = _number(str[6]);
                int32_t g = _number(str[7]);
                int32_t h = _number(str[8]);
                return (a << 28) | (b << 24) | (c << 20) | (d << 16) | (e << 12) | (f << 8) | (g << 4) | (h << 0);
            }

            return colors::Auto;
        }
        for (auto & color_name : core::color_names)
        {
            if (core::string::equal_ic(str, color_name.name, -1))
                return color_name.color;
        }
        return colors::Auto;
    }

    core::vec2<core::color32> parseColor2D(const std::string & str)
    {
        std::vector<std::string> strs = core::string::split(str, ' ');
        if (strs.size() == 1)
            return { parseColor(strs[0])};
        if (strs.size() == 2)
            return { parseColor(strs[0]), parseColor(strs[1]) };
        return {};
    }

    core::vec4<core::color32> parseColor4D(const std::string & str)
    {
        std::vector<std::string> strs = core::string::split(str, ' ');
        if (strs.size() == 1)
            return { parseColor(strs[0])};
        if (strs.size() == 2)
            return { parseColor(strs[0]), parseColor(strs[1]) };
        if (strs.size() == 4)
            return { parseColor(strs[0]), parseColor(strs[1]), parseColor(strs[2]), parseColor(strs[3]) };
        return {};
    }

    core::dimensionf parseDimension(const std::string & str)
    {
        for (auto & uint_name : unit_names)
        {
            size_t pos = str.rfind(uint_name.name, -1);
            if (pos != std::string::npos)
                return core::dimensionf(core::texttof32(str.data(), pos), uint_name.unit);
        }

        return {};
    }

    core::vec2<core::dimensionf> parseDimension2D(const std::string & str)
    {
        std::vector<std::string> strs = core::string::split(str, ' ');
        if (strs.size() == 1)
            return { parseDimension(strs[0]) };
        if (strs.size() == 2)
            return { parseDimension(strs[0]), parseDimension(strs[1]) };
        return {};
    }

    core::vec4<core::dimensionf> parseDimension4D(const std::string & str)
    {
        std::vector<std::string> strs = core::string::split(str, ' ');
        if (strs.size() == 1)
            return { parseDimension(strs[0]) };
        if (strs.size() == 2)
            return { parseDimension(strs[0]), parseDimension(strs[1]) };
        if (strs.size() == 4)
            return { parseDimension(strs[0]), parseDimension(strs[1]), parseDimension(strs[2]), parseDimension(strs[3]) };
        return {};
    }

    std::chrono::nanoseconds parseDuration(const std::string & str)
    {
        auto end = str.end();
        char * curr = nullptr;
        int64_t val = std::strtoll(str.c_str(), &curr, 10);
        if (errno == ERANGE)
            return {};

        if (curr && curr[0] == 's')
            return std::chrono::seconds(val);
        return std::chrono::milliseconds(val);
    }

    void property_animation::update()
    {
        if (_state != animation_state::running)
            return;

        auto object = _object.lock();
        if (!object || !_accessor || !_interpolator)
            return;

        auto now = core::datetime::steady();
        auto cost = now - _time;
        if (cost > _duration * _loop_index + _duration)
        {
            ++_loop_index;
            if (_loop <= 0 || _loop_index >= _loop)
            {
                _state = animation_state::waiting;
                stoped();
                return;
            }
            looped(_loop_index);
        }

        float32_t proportion = (cost - _duration * _loop_index).count() / (float32_t)_duration.count();
        proportion = _curve(proportion);
        _interpolator->interpolate(*object, *_accessor, proportion);
    }
}