#include "stdafx.h"
#include "app.h"

namespace core
{
    IApp * __app = nullptr;


    const core::property_table & IApp::properties(const std::type_info & ti, std::function<void(core::property_table &)> callback)
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

    void app(IApp * ptr)
    {
        __app = ptr;
    }

    IApp & app()
    {
        return *__app;
    }
}
