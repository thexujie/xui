#include "stdafx.h"
#include "invokable.h"
#include <win32/windows.h>

namespace core
{
    static thread_local invokable_helper __helper;

    invokable_helper & invokable_get_helper()
    {
        return __helper.ref();
    }

    void invokable_clear()
    {
        __helper.clear();
    }

    static void CALLBACK InvokerAPCCallBack(ULONG_PTR dwParam)
    {
        __helper.trigger();
    }

    invokable_helper::invokable_helper()
    {
        _id = GetCurrentThreadId();
        _thread = OpenThread(THREAD_SET_CONTEXT, FALSE, _id);

        if(!_thread)
            logger::err() << __FUNCTION__" OpenThread" << win32::winerr_str(GetLastError());
    }
    invokable_helper::~invokable_helper()
	{
        if(_invokers)
        {
            delete _invokers;
            _invokers = nullptr;
        }
        if(_invoker_mutex)
        {
            delete _invoker_mutex;
            _invoker_mutex = nullptr;
        }
        if(_thread)
        {
            CloseHandle(_thread);
            _thread = nullptr;
            _id = 0;
        }
    }

    invokable_helper & invokable_helper::ref()
    {
        if(_id != GetCurrentThreadId())
            throw 0;

        if(!_invokers)
            _invokers = new std::list<std::weak_ptr<iinvokable>>;
        if(!_invoker_mutex)
            _invoker_mutex = new std::mutex();

        return *this;
    }

    error invokable_helper::add(std::weak_ptr<iinvokable> invoker)
    {
        if(!_thread)
            return error_state;
        std::lock_guard<std::mutex> lock(*_invoker_mutex);
        _invokers->push_back(invoker);
        ::QueueUserAPC(InvokerAPCCallBack, (HANDLE)_thread, 0);
        return error_ok;
    }

    void invokable_helper::clear()
    {
        if(_invokers)
        {
            delete _invokers;
            _invokers = nullptr;
        }
        if(_invoker_mutex)
        {
            delete _invoker_mutex;
            _invoker_mutex = nullptr;
        }
        if(_thread)
        {
            CloseHandle(_thread);
            _thread = nullptr;
            _id = 0;
        }
    }

    error invokable_helper::trigger()
    {
        if(_id != GetCurrentThreadId())
            return error_state;

        std::list<std::weak_ptr<iinvokable>> invokers;
        {
            std::lock_guard<std::mutex> lock(*_invoker_mutex);
            invokers = std::move(*_invokers);
        }

        for(auto invoker : invokers)
        {
            auto sp_invoker = invoker.lock();
            if(sp_invoker)
                sp_invoker->trigger();
        }

        return error_ok;
    }

}
