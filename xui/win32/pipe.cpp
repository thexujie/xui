#include "stdafx.h"
#include "pipe.h"
#include "windows.h"

namespace win32
{
    struct ipie_iocp_context
    {
        HANDLE hEvent;
    };

    static void WINAPI pipe_iocp_callback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
    {
        //ipie_iocp_context * context = (ipie_iocp_context *)lpOverlapped->hEvent;
        //if(context)
        //    SetEvent(context->hEvent);
        SetEvent(lpOverlapped->hEvent);
    }

    using namespace common;

    pipe::pipe()
    {
        
    }

    pipe::pipe(std::string uri, pipe_mode mode, std::chrono::microseconds timeout)
    {
        connect(uri, mode, timeout);
    }

    pipe::~pipe()
    {
        if ((_mode & pipe_mode_async) && _ovlp_r)
        {
            OVERLAPPED * ovlp = (OVERLAPPED *)_ovlp_r.get();
            ::CancelIoEx(_pipe.get(), ovlp);
            DWORD nbRead = 0;
            bool bSuccess = GetOverlappedResult(_pipe.get(), (OVERLAPPED *)_ovlp_r.get(), &nbRead, TRUE);
            DWORD dwErr = GetLastError();
            assert(bSuccess || dwErr == ERROR_BROKEN_PIPE);
            if (!bSuccess)
            {
                logger::err() << __FUNCTION__" GetOverlappedResult " << _uri << " " << winerr_str(GetLastError());
            }
        }
    }

    common::error_e pipe::state() const
    {
        return _pipe ? error_ok : error_broken;
    }

    std::shared_ptr<sync::event> pipe::iocp_event_r() const
    {
        if ((_mode & pipe_mode_async_r) != pipe_mode_async_r)
            return nullptr;

        if (!_iocp_event_r)
            const_cast<pipe *>(this)->_iocp_event_r = std::make_shared<sync::event>(false, true);
        return _iocp_event_r;
    }

    std::shared_ptr<sync::event> pipe::iocp_event_w() const
    {
        if ((_mode & pipe_mode_async_w) != pipe_mode_async_w)
            return nullptr;

        if (!_iocp_event_w)
            const_cast<pipe *>(this)->_iocp_event_w = std::make_shared<sync::event>(false, true);
        return _iocp_event_w;
    }

    error_e pipe::connect(std::string uri, pipe_mode mode, std::chrono::microseconds timeout)
    {
        std::wstring name = tools::string::u8_ucs2(uri);
        if (!WaitNamedPipeW(name.c_str(), timeout.count() / 1000))
        {
            assert(false);
            logger::err() << __FUNCTION__" WaitNamedPipeW " << uri << " " << winerr_str(GetLastError());
            return error_inner;
        }

        DWORD dwAccess = 0;
        if (mode & pipe_mode::pipe_mode_rw)
            dwAccess = PIPE_ACCESS_INBOUND | GENERIC_WRITE;
        else if (mode & pipe_mode::pipe_mode_r)
            dwAccess = GENERIC_READ;
        else if (mode & pipe_mode::pipe_mode_w)
            dwAccess = GENERIC_WRITE;
        else{}

        HANDLE hPipe = CreateFileW(name.c_str(), dwAccess, 0, NULL, OPEN_EXISTING, (mode & pipe_mode::pipe_mode_async) ? FILE_FLAG_OVERLAPPED : 0, NULL);
        assert(hPipe != INVALID_HANDLE_VALUE);
        if (hPipe == INVALID_HANDLE_VALUE)
        {
            logger::err() << __FUNCTION__" CreateFileW " << uri << " " << winerr_str(GetLastError());
            return error_inner;
        }
        std::shared_ptr<byte_t> __hPipe((byte_t *)hPipe, [](byte_t * ptr) { ::CloseHandle((HANDLE)ptr); });

        _pipe = __hPipe;
        _mode = mode;
        _uri = uri;
        return error_ok;
    }

    common::error_e pipe::iocp_result_r()
    {
        if (!_pipe)
            return error_state;

        if ((_mode & pipe_mode_async) && !_ovlp_r)
            return error_state;

        DWORD nbRead = 0;
        bool bSuccess = GetOverlappedResult(_pipe.get(), (OVERLAPPED *)_ovlp_r.get(), &nbRead, FALSE);
        DWORD dwErr = GetLastError();
        if (!bSuccess)
        {
            if (dwErr == ERROR_IO_INCOMPLETE)
                return error_pendding;

            _buffer_r.reset();
            _size_r = 0;

            switch(dwErr)
            {
            case ERROR_BROKEN_PIPE:
                return error_broken;
            case ERROR_HANDLE_EOF:
                return error_eof;
            default:
                logger::err() << __FUNCTION__" GetOverlappedResult " << _uri << " " << winerr_str(dwErr);
                return error_inner;
            }
        }

        _buffer_r.reset();
        _size_r = 0;
        return error_ok;
    }

    common::error_e pipe::iocp_result_w()
    {
        if (!_pipe)
            return error_state;

        if ((_mode & pipe_mode_async) && !_ovlp_w)
            return error_state;

        DWORD nbWriten = 0;
        bool bSuccess = GetOverlappedResult(_pipe.get(), (OVERLAPPED *)_ovlp_w.get(), &nbWriten, FALSE);
        DWORD dwErr = GetLastError();
        if (!bSuccess)
        {
            if (dwErr == ERROR_IO_INCOMPLETE)
                return error_pendding;

            _buffer_w.reset();
            _size_w = 0;

            switch (dwErr)
            {
            case ERROR_BROKEN_PIPE:
                return error_broken;
            case ERROR_HANDLE_EOF:
                return error_eof;
            default:
                logger::err() << __FUNCTION__" GetOverlappedResult " << _uri << " " << winerr_str(dwErr);
                return error_inner;
            }
        }

        _buffer_w.reset();
        _size_w = 0;
        return error_ok;
    }

    common::error_e pipe::iocp_read(std::shared_ptr<byte_t> buffer, int64_t nbytes)
    {
        if (!_pipe || !(_mode & pipe_mode_r))
            return error_state;

        OVERLAPPED * ovlp = nullptr;
        if (_mode & pipe_mode::pipe_mode_async)
        {
            if(!_ovlp_r)
            {
                _ovlp_r.reset(new byte_t[sizeof(OVERLAPPED)], std::default_delete<byte_t[]>());
                std::memset(_ovlp_r.get(), 0, sizeof(OVERLAPPED));
                ovlp = (OVERLAPPED *)_ovlp_r.get();
                ovlp->hEvent = iocp_event_r()->handle();
            }
            else
                ovlp = (OVERLAPPED *)_ovlp_r.get();
            ovlp->Offset = 0;
            ovlp->OffsetHigh = 0;
        }

        bool bSuccess = ReadFileEx(_pipe.get(), buffer.get(), (DWORD)nbytes, ovlp, pipe_iocp_callback);
        if (bSuccess)
        {
            _buffer_r = buffer;
            _size_r = nbytes;
            return error_ok;
        }

        logger::err() << __FUNCTION__" ReadFileEx " << _uri << " " << winerr_str(GetLastError());
        return error_io;
    }

    common::error_e pipe::iocp_send(std::shared_ptr<byte_t> buffer, int64_t nbytes)
    {
        if (!_pipe || !(_mode & pipe_mode_w))
            return error_state;

        OVERLAPPED * ovlp = nullptr;
        if (_mode & pipe_mode::pipe_mode_async)
        {
            if (!_ovlp_w)
            {
                _ovlp_w.reset(new byte_t[sizeof(OVERLAPPED)], std::default_delete<byte_t[]>());
                std::memset(_ovlp_w.get(), 0, sizeof(OVERLAPPED));
                ovlp = (OVERLAPPED *)_ovlp_w.get();
                ovlp->hEvent = iocp_event_w()->handle();
            }
            else
                ovlp = (OVERLAPPED *)_ovlp_w.get();
            ovlp->Offset = 0;
            ovlp->OffsetHigh = 0;
        }

        bool bSuccess = WriteFileEx(_pipe.get(), buffer.get(), (DWORD)nbytes, ovlp, pipe_iocp_callback);
        if (bSuccess)
        {
            _buffer_w = buffer;
            _size_w = nbytes;
            return error_ok;
        }

        logger::err() << __FUNCTION__" WriteFileEx " << _uri << " " << winerr_str(GetLastError());
        return error_io;
    }
}
