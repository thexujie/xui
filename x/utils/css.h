#pragma once

namespace utils
{
    struct css_item
    {
        std::string key;
        std::string value;
    };

    struct css_style
    {
        std::string name;
        std::map<std::string, std::string> items;
    };

    class CSS
    {
    public:
        CSS();
        ~CSS();

        core::error loadFromData(std::string data);

        std::map<std::string, std::map<std::string, std::string>> _items;
    };
}
