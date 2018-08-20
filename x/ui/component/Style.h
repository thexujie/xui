#pragma once

namespace ui::component
{
    class Style
    {
    public:
        std::shared_ptr<Style> get(std::vector<std::string>::iterator iter, std::vector<std::string>::iterator end);
        std::shared_ptr<Style> get(const std::string & name);
        std::shared_ptr<Style> select(std::vector<std::string>::iterator iter, std::vector<std::string>::iterator end);
        std::shared_ptr<Style> select(const std::string & name);

        // button button:hover button:active
        std::map<std::string, std::string> generate(const std::string & selector);

        void join(Style & another);
        void join(const std::map<std::string, std::shared_ptr<Style>> & styles);

        void set(const std::string & key, const std::string & value) { items[key] = value; }
        std::string & operator[](const std::string & key) { return items[key]; }

        std::map<std::string, std::string> items;
        std::map<std::string, std::map<std::string, std::string>> pseudos;
        std::map<std::string, std::shared_ptr<Style>> styles;
    };

    class StyleSheet : public Style
    {
    public:
        core::error loadFromFile(std::string path);
        core::error loadFromData(const std::string & data);

    private:
        std::shared_ptr<StyleSheet> _next;
    };
}
