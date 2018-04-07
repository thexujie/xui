#pragma once
#include "core/app.h"
#include "GraphicsService.h"

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
            if (name != "GraphicsService")
                return nullptr;

            if (!_graphicsService)
                _graphicsService = std::make_shared<win32::GraphicsService>();
            return std::dynamic_pointer_cast<core::Object>(_graphicsService);
        }

    private:
        std::shared_ptr<win32::GraphicsService> _graphicsService;
    };
}
