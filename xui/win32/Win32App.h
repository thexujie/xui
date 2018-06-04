#pragma once
#include "core/app.h"
#include "GraphicsService.h"
#include "uniscribe/ScriptService.h"

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

        std::shared_ptr<core::Object> GetService(std::string name)
        {
            if (name == "GraphicsService")
            {
                if (!_graphicsService)
                    _graphicsService = std::make_shared<win32::GraphicsService>();
                return std::dynamic_pointer_cast<core::Object>(_graphicsService);
            }

            if (name == "ScriptService")
            {
                if (!_scriptService)
                    _scriptService = std::make_shared<win32::uniscribe::ScriptService>();
                return std::dynamic_pointer_cast<core::Object>(_scriptService);
            }
            return nullptr;
        }

    private:
        std::shared_ptr<win32::GraphicsService> _graphicsService;
        std::shared_ptr<win32::uniscribe::ScriptService> _scriptService;
    };
}
