#include "stdafx.h"
#include "Win32Invokable.h"
#include "windows.h"
#include <processthreadsapi.h>

namespace win32
{
    struct Win32InvokableData
    {
        std::weak_ptr<tools::invokable> invokable;
    };

    void CALLBACK APCCallBack(ULONG_PTR dwParam)
    {
        Win32InvokableData * data = (Win32InvokableData *)dwParam;
        std::shared_ptr<tools::invokable> invokable = data->invokable.lock();
        delete data;

        if (invokable)
            invokable->doInvoke();
    }

    Win32Invokable::Win32Invokable()
    {
        HANDLE hThread = OpenThread(THREAD_SET_CONTEXT, FALSE, GetCurrentThreadId());
        if (!hThread)
            common::logger::err() << __FUNCTION__" OpenThread" << winerr_str(GetLastError());
        else
            _hThread.reset(hThread, [](void * ptr){ ::CloseHandle((HANDLE)ptr); });
    }

    Win32Invokable::~Win32Invokable()
    {
        
    }

    void Win32Invokable::invoke_notify()
    {
        if (_hThread)
        {
            Win32InvokableData * data = new Win32InvokableData();
            data->invokable = tools::invokable::shared_from_this();
            ::QueueUserAPC(APCCallBack, (HANDLE)_hThread.get(), (ULONG_PTR)data);
        }
    }
}
