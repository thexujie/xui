#pragma once

namespace ui::component
{
    class Style
    {
    public:
        virtual ~Style() = default;
        std::shared_ptr<Style> get(std::vector<std::u8string>::iterator iter, std::vector<std::u8string>::iterator end);
        std::shared_ptr<Style> get(const std::u8string & name);
        std::shared_ptr<Style> select(std::vector<std::u8string>::iterator iter, std::vector<std::u8string>::iterator end);
        std::shared_ptr<Style> select(const std::u8string & name);

        // button button:hover button:active
        std::map<std::u8string, std::u8string> generate(const std::u8string & selector);

        void join(Style & another);
        void join(const std::map<std::u8string, std::shared_ptr<Style>> & styles);

        void set(const std::u8string & key, const std::u8string & value) { items[key] = value; }
        std::u8string & operator[](const std::u8string & key) { return items[key]; }

        std::map<std::u8string, std::u8string> items;
        std::map<std::u8string, std::map<std::u8string, std::u8string>> pseudos;
        std::map<std::u8string, std::shared_ptr<Style>> styles;
    };

    class StyleSheet : public Style
    {
    public:
        core::error loadFromFile(const std::u8string & path);
        core::error loadFromData(const std::u8string & data);

    private:
        std::shared_ptr<StyleSheet> _next;
    };
}
