#pragma once
#include "tools/invokable.h"

namespace win32
{
    class Win32Invokable : public tools::invokable
    {
    public:
        Win32Invokable();
        ~Win32Invokable();

    private:
        void invoke_notify();

    private:
        std::shared_ptr<void> _hThread;
    };
}
