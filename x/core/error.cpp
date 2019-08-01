#include "stdafx.h"
#include "error.h"

#define ERROR_TAG(tag) tag, #tag

namespace core
{
    struct error_entry
    {
        int num;
        const char * tag;
        const char * str;
    };

    static const struct error_entry error_entries[] =
    {
        { ERROR_TAG(e_generic), "generic error" },
        { ERROR_TAG(e_inner), "inner error" },
        { ERROR_TAG(e_eof), "end of stream" },
        { ERROR_TAG(e_nullptr), "null pointer" },
        { ERROR_TAG(e_unreachable), "unreachable" },
        { ERROR_TAG(e_outofmemory), "out of memory" },
        { ERROR_TAG(e_outofbound), "out of bound" },
        { ERROR_TAG(e_args), "invalid args" },
        { ERROR_TAG(e_exists), "not exists" },
        { ERROR_TAG(e_access), "no access" },
        { ERROR_TAG(e_state), "invalid state" },
        { ERROR_TAG(e_io), "io error" },
        { ERROR_TAG(e_broken), "broken" },
        { ERROR_TAG(e_timeout), "timeout" },
        { ERROR_TAG(e_cancel), "cancel" },
        { ERROR_TAG(e_bad_data), "bad data" },
        { ERROR_TAG(e_bad_format), "bad format" },
        { ERROR_TAG(e_not_now), "not now" },
        { ERROR_TAG(e_not_supported), "not supported" },
        { ERROR_TAG(e_not_supported), "not found" },
        { ERROR_TAG(e_invalid_operation), "invalid operation" },
        { ERROR_TAG(ok), "ok" },
        { ERROR_TAG(ok_true), "true" },
        { ERROR_TAG(ok_false), "false" },
        { ERROR_TAG(ok_pendding), "pendding" },
    };

    const char * error_str(error err)
    {
        for (const error_entry & entry : error_entries)
        {
            if (err == entry.num)
                return entry.str;
        }

        return "unknown error";
    }
}
