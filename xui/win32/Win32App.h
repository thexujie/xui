#pragma once
#include "core/app.h"
#include "GraphicsService.h"
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
            if (name == "GraphicsService")
            {
                if (!_graphicsService)
                    _graphicsService = std::make_shared<win32::GraphicsService>();
                return std::dynamic_pointer_cast<core::object>(_graphicsService);
            }

            if (name == "ScriptService")
            {
                if (!_scriptService)
                    _scriptService = std::make_shared<win32::uniscribe::ScriptService>();
                return std::dynamic_pointer_cast<core::object>(_scriptService);
            }
            return nullptr;
        }

        void quit(int32_t ret) { win32::quit(0); }
    private:
        std::shared_ptr<win32::GraphicsService> _graphicsService;
        std::shared_ptr<win32::uniscribe::ScriptService> _scriptService;
    };
}
