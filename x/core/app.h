#pragma once

namespace core
{
    class IApp : public core::object
    {
    public:
        virtual ~IApp() = default;
        virtual std::shared_ptr<object> GetService(std::string name) = 0;
        virtual void quit(int32_t ret) = 0;

        const core::property_table & properties(const std::type_info & ti, std::function<void(core::property_table &)> callback);
        template<typename T>
        const core::property_table & properties()
        {
            return properties(typeid(T), T::propertyTableCallback);
        }

    protected:
        std::mutex _mtx;
        std::map<const std::type_info *, std::shared_ptr<core::property_table>> _properties;
    };

    void app(IApp * ptr);
    IApp & app();
}
