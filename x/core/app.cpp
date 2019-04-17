#include "stdafx.h"
#include "app.h"

#include "win32/win32.h"

namespace core
{
    App * __app = nullptr;

    App::App()
    {
        __app = this;
    }

    App::~App()
    {
        __app = nullptr;
    }

    const core::property_table & App::properties(const std::type_info & ti, std::function<void(core::property_table &)> callback)
    {
        std::lock_guard < std::mutex > lock(_mtx);
        auto iter = _properties.find(&ti);
        if (iter == _properties.end())
        {
            auto table = std::make_shared<core::property_table>();
            callback(*table);
            _properties[&ti] = table;
            return *table;
        }
        else
            return *(iter->second);
    }

    void app(App * ptr)
    {
        __app = ptr;
    }

    std::shared_ptr<object> App::GetService(std::string name)
    {
        return nullptr;
    }

    void App::quit(int32_t ret)
    {
        win32::endLoop(ret);
    }

    App & app()
    {
        return *__app;
    }
}
