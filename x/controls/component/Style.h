#pragma once

namespace controls::component
{
    struct StyleClass
    {
    public:
        std::map<std::string, std::shared_ptr<StyleClass>> attrs;
    };

    class Style
    {
    public:
        std::string name;
        std::map<std::string, std::shared_ptr<StyleClass>> subclasses;
    };

    class StyleSheet
    {
    public:
        std::shared_ptr<Style> select(std::string name);

        std::shared_ptr<StyleSheet> _next;
    };
}