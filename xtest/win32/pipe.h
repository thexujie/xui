#pragma once

#include "tools/io/stream.h"
#include "sync.h"

namespace win32
{
    enum pipe_mode
    {
        pipe_mode_r = 0x0001,
        pipe_mode_w = 0x0002,
        pipe_mode_rw = pipe_mode_r | pipe_mode_w,
        pipe_mode_async = 0x0010,
        pipe_mode_async_r = pipe_mode_r | pipe_mode_async,
        pipe_mode_async_w = pipe_mode_w | pipe_mode_async,
        pipe_mode_async_rw = pipe_mode_rw | pipe_mode_async,
    };

    class pipe
    {
    public:
        pipe();
        pipe(std::string uri, pipe_mode mode, std::chrono::microseconds timeout);
        ~pipe();

        common::error_e state() const;
        std::shared_ptr<sync::event> iocp_event_r() const;
        std::shared_ptr<sync::event> iocp_event_w() const;

        common::error_e connect(std::string uri, pipe_mode mode, std::chrono::microseconds timeout);

        common::error_e iocp_result_r();
        common::error_e iocp_result_w();
        common::error_e iocp_read(std::shared_ptr<byte_t> buffer, int64_t nbytes);
        common::error_e iocp_send(std::shared_ptr<byte_t> buffer, int64_t nbytes);

    private:
        std::string _uri;
        pipe_mode _mode = pipe_mode::pipe_mode_r;
        std::shared_ptr<byte_t> _pipe;
        std::shared_ptr<byte_t> _ovlp_r;
        std::shared_ptr<byte_t> _ovlp_w;
        std::shared_ptr<sync::event> _iocp_event_r;
        std::shared_ptr<sync::event> _iocp_event_w;
        int32_t _size_r = 0;
        int32_t _size_w = 0;
        std::shared_ptr<byte_t> _buffer_r;
        std::shared_ptr<byte_t> _buffer_w;
    };

    class pipe_server
    {
    public:
        pipe_server();
        ~pipe_server();

    private:
    };
}
