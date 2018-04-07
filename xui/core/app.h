#pragma once

namespace core
{
    class IApp
    {
    public:
        virtual ~IApp() = default;
        virtual std::shared_ptr<Object> GetService(std::string name) = 0;
    };

    void app(IApp * ptr);
    IApp & app();
}
