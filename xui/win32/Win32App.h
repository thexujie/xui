#pragma once
#include "core/app.h"
#include "win32/win32.h"

namespace win32
{
    class Win32App : public core::IApp
    {
    public:
        Win32App()
        {
            core::app(this);
        }

        ~Win32App()
        {
            core::app(nullptr);
        }

        std::shared_ptr<core::object> GetService(std::string name)
        {
            return nullptr;
        }

        void quit(int32_t ret) { async([ret]() {win32::endLoop(ret); }); }
    };
}
