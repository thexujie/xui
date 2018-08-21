#include "stdafx.h"
#include "object.h"
#include "win32/windows.h"

namespace core
{
    static thread_local invokable_helper __helper;
    static std::atomic_uint64_t __object_id = 1;

    uint64_t object::create_objectid()
    {
        static const uint64_t _one = 1;
        uint64_t val = __object_id.fetch_add(_one);
        //return std::hash_value<uint64_t>(val);
        return val;
    }

    invokable_helper & object::invokable_get_helper()
    {
        return __helper.ref();
    }

    static void CALLBACK InvokerAPCCallBack(ULONG_PTR dwParam)
    {
        __helper.trigger();
    }

    invokable_helper::invokable_helper()
    {
        _id = GetCurrentThreadId();
    }
    invokable_helper::~invokable_helper()
    {
        if (_thread)
        {
            CloseHandle(_thread);
            _thread = nullptr;
            _id = 0;
        }
    }

    invokable_helper & invokable_helper::ref()
    {
        if (_id != GetCurrentThreadId())
            throw 0;

        return *this;
    }

    error invokable_helper::add(std::shared_ptr<object> invoker, std::function<void()> fun)
    {
        if (!_id)
            return error_state;
        std::lock_guard<std::mutex> lock(_mtx);
        if (!_thread)
        {
            _thread = OpenThread(THREAD_SET_CONTEXT, FALSE, _id);
            if (!_thread)
            {
                logger::err() << __FUNCTION__" OpenThread" << win32::winerr_str(GetLastError());
                _thread = INVALID_HANDLE_VALUE;
            }
        }
        if (!_thread || _thread == INVALID_HANDLE_VALUE)
            return error_state;

        _invokers[invoker].push_back(fun);
        ::QueueUserAPC(InvokerAPCCallBack, (HANDLE)_thread, 0);
        return error_ok;
    }

    error invokable_helper::trigger()
    {
        if (_id != GetCurrentThreadId())
            return error_state;

        invoker_map invokers;
        {
            std::lock_guard<std::mutex> lock(_mtx);
            invokers = std::move(_invokers);
        }

        for (invoker_map::iterator iter = invokers.begin(); iter != invokers.end(); ++iter)
        {
            auto invoker = iter->first.lock();
            if (!invoker)
                continue;

            for (auto & fun : iter->second)
                fun();
        }

        return error_ok;
    }

    error object::invoke(std::function<void()> fun)
    {
        return _invoker_helper.add(shared_from_this(), fun);
    }
}