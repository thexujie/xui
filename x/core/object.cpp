#include "stdafx.h"
#include "object.h"
#include "win32/windows.h"

namespace core
{
    static thread_local invoke_helper __helper;
    static std::atomic_uint64_t __object_id = 1;

    uint64_t object::create_objectid()
    {
        static const uint64_t _one = 1;
        uint64_t val = __object_id.fetch_add(_one);
        //return std::hash_value<uint64_t>(val);
        return val;
    }

    invoke_helper & object::get_invoke_helper()
    {
        return __helper.ref();
    }

    static void CALLBACK InvokerAPCCallBack(ULONG_PTR dwParam)
    {
        __helper.trigger();
    }

    invoke_helper::invoke_helper()
    {
        _id = GetCurrentThreadId();
    }
    invoke_helper::~invoke_helper()
    {
        std::unique_lock<std::mutex> lock(_mtx);
        if (_thread)
        {
            CloseHandle(_thread);
            _thread = nullptr;
            _id = 0;
        }
        lock.unlock();
    }

    invoke_helper & invoke_helper::ref()
    {
        if (_id != GetCurrentThreadId())
            throw 0;

        return *this;
    }

    bool invoke_helper::can_safe_invoke() const
    {
        return ::GetCurrentThreadId() == _id;
    }

    void invoke_helper::check_invoke()
    {
        if (GetCurrentThreadId() != _id)
            throw core::exception(core::error_invalid_operation);
    }

    void * invoke_helper::thread_handle() const
    {
        if (!_id)
            return nullptr;

        std::lock_guard<std::mutex> lock(_mtx);
        if (!_thread)
        {
            _thread = OpenThread(THREAD_SET_CONTEXT, FALSE, _id);
            if (!_thread)
            {
                logger::err() << __FUNCTION__" OpenThread" << win32::winerr_str(GetLastError());
            }
        }
        return _thread;
    }

    error invoke_helper::add(std::shared_ptr<object> invoker, std::function<void()> fun)
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
            }
        }
        if (!_thread)
            return error_state;

        _invokers[invoker].push_back(fun);
        ::QueueUserAPC(InvokerAPCCallBack, (HANDLE)_thread, 0);
        return error_ok;
    }

    error invoke_helper::add(std::shared_ptr<object> invoker, std::shared_ptr<invoke_task> task)
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
            }
        }
        if (!_thread)
            return error_state;

        _tasks[invoker].push_back(task);
        ::QueueUserAPC(InvokerAPCCallBack, (HANDLE)_thread, 0);
        return error_ok;
    }

    error invoke_helper::trigger()
    {
        if (_id != GetCurrentThreadId())
            return error_state;

        invoker_map invokers;
        task_map tasks;
        while(true)
        {
            {
                std::lock_guard<std::mutex> lock(_mtx);
                invokers = std::move(_invokers);
                tasks = std::move(_tasks);
            }

            if (invokers.empty() && tasks.empty())
                break;

            for (invoker_map::iterator iter = invokers.begin(); iter != invokers.end(); ++iter)
            {
                auto invoker = iter->first.lock();
                if (!invoker)
                    continue;

                for (auto & fun : iter->second)
                    fun();
            }

            for (task_map::iterator iter = tasks.begin(); iter != tasks.end(); ++iter)
            {
                auto invoker = iter->first.lock();
                if (!invoker)
                    continue;

                for (auto & task : iter->second)
                    task->trigger();
            }

        }
        return error_ok;
    }
}