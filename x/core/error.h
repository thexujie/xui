#pragma once

namespace core
{
    enum error : int32_t
    {
        e_generic = std::numeric_limits<int32_t>::min(),
        e_inner,
        e_eof,
        e_failed,
        e_nullptr,
        e_unreachable,
        e_outofmemory,
        e_outofbound,
        e_args,
        e_exists,
        e_access,
        e_state,
        e_io,
        e_broken,

        e_timeout,
        e_cancel,

        e_bad_data,
        e_bad_format,

        e_not_now,
        e_not_supported,
        e_not_found,

        e_invalid_operation,

        ok = 0,
        ok_true,
        ok_false,
        ok_pendding,
    };

    enum class flag
    {
        none = 0,
        expired = 0x0001,
        paused = 0x0002,
        stoped = 0x0004,
        canceled = 0x0004,
    };
    typedef bitflag<flag> flags;

    const char * error_str(error err);

    class exception : public std::exception
    {
    public:
        exception(const char * msg) : std::exception(msg) {}
        exception(error err) : std::exception(error_str(err)) {}
    };
}
