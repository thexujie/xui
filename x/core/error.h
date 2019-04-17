#pragma once

namespace core
{
    enum error : int32_t
    {
        error_generic = std::numeric_limits<int32_t>::min(),
        error_inner,
        error_eof,
        error_failed,
        error_nullptr,
        error_unreachable,
        error_outofmemory,
        error_outofbound,
        error_args,
        error_exists,
        error_access,
        error_state,
        error_io,
        error_broken,

        error_timeout,
        error_cancel,

        error_bad_data,
        error_bad_format,

        error_not_now,
        error_not_supported,
        error_not_found,

        error_invalid_operation,

        error_ok = 0,
        error_true,
        error_false,
        error_pendding,
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
