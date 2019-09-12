#include "stdafx.h"
#include "sync.h"
#include "windows.h"

namespace win32
{
    namespace sync
    {
        event::event() : _handle(CreateEvent(NULL, FALSE, FALSE, NULL))
        {
        }
        event::event(bool manual, bool set) : _handle(CreateEvent(NULL, manual ? TRUE : FALSE, set ? TRUE : FALSE, NULL))
        {
        }

        void event::set()
        {
            ::SetEvent((HANDLE)_handle);
        }

        void event::reset()
        {
            ::ResetEvent((HANDLE)_handle);
        }
    }
}
