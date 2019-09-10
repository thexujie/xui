#include "stdafx.h"
#include "object.h"

namespace core
{
    static std::atomic_uint64_t __object_id = 1;
	static thread_local std::unique_ptr<invoke_helper> __helper;

    uint64_t object::create_objectid()
    {
        static const uint64_t _one = 1;
        uint64_t val = __object_id.fetch_add(_one);
        //return std::hash_value<uint64_t>(val);
        return val;
    }

    invoke_helper & object::get_invoke_helper()
    {
        if (!__helper)
            __helper.reset(new invoke_helper());
        return *__helper;
    }
}