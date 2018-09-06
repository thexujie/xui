#pragma once
#include "core/app.h"
#include "uniscribe/ScriptService.h"
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
            if (name == "ScriptService")
            {
                if (!_scriptService)
                    _scriptService = std::make_shared<win32::uniscribe::ScriptService>();
                return std::dynamic_pointer_cast<core::object>(_scriptService);
            }
            return nullptr;
        }

        void quit(int32_t ret) { async([ret]() {win32::endLoop(ret); }); }
    private:
        std::shared_ptr<win32::uniscribe::ScriptService> _scriptService;
    };
}
