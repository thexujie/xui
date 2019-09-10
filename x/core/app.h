#pragma once

namespace core
{
    class App : public core::object
    {
    public:
        App();
        virtual ~App();
        virtual std::shared_ptr<object> get_service(std::string name);
        virtual void quit(int32_t ret);

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

    void app(App * ptr);
    App & app();
}
