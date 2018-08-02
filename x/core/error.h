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


        error_ok = 0,
        error_true,
        error_false,
        error_pendding,
    };

    enum flag_e
    {
        flag_none = 0,
        flag_pause = 0x0001,
        flag_stop = 0x0002,
        flag_clear = 0x0004,
    };

    const char * error_str(error err);

    class exception : public std::exception
    {
    public:
        exception(const char * msg) : std::exception(msg) {}
        exception(error err) : std::exception(error_str(err)) {}
    };
}
