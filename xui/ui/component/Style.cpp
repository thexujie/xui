#include "stdafx.h"
#include "Style.h"

namespace ui::component
{
    struct style_string_walker
    {
        style_string_walker(const std::u8string & str_) : str(str_) {}
        const std::u8string & str;
        int32_t index = 0;

        bool eof() const { return index >= str.length(); }

        char read()
        {
            if (eof())
                return 0;

            return str[index++];
        }

        char peek()
        {
            if (eof())
                return 0;

            return str[index];
        }

        std::u8string readTo(char ch)
        {
            std::u8string ret;
            while (!eof())
            {
                char c = read();
                if (c == ch)
                    break;

                ret.append(1, c);
            }
            return ret;
        }

        std::u8string readTo(char chs[], int num, char & end)
        {
            std::u8string ret;
            while (!eof())
            {
                char ch = read();
                char * pos = std::find(chs, chs + num, ch);
                if (pos != (chs + num))
                {
                    end = *pos;
                    break;
                }
                ret.append(1, ch);
            }
            return ret;
        }

        void skip()
        {
            while (!eof())
            {
                if (!isspace(peek()))
                    break;

                read();
            }
        }
    };

    std::shared_ptr<Style> Style::select(std::vector<std::u8string>::iterator iter, std::vector<std::u8string>::iterator end)
    {
        auto style_iter = styles.find(*iter);
        if (style_iter == styles.end())
            return nullptr;

        ++iter;
        if (iter == end)
            return style_iter->second;
        return style_iter->second->select(iter, end);
    }

    std::shared_ptr<Style> Style::select(const std::u8string & name)
    {
        std::vector<std::u8string> ids = core::split(name, u8' ');
        return select(ids.begin(), ids.end());
    }

    std::map<std::u8string, std::u8string> Style::generate(const std::u8string & selector)
    {
        if (selector.empty())
            return {};

        std::vector<std::u8string> ids = core::split(selector, u8':');
        auto style = select(ids[0]);
        if (!style)
            return {};

        std::map<std::u8string, std::u8string> items = style->items;
        if (ids.size() < 2)
            return items;

        auto iter = style->pseudos.find(ids[1]);
        if(iter != style->pseudos.end())
        {
            for (auto & item : iter->second)
                items[item.first] = item.second;
        }
        return items;
    }

    std::shared_ptr<Style> Style::get(std::vector<std::u8string>::iterator iter, std::vector<std::u8string>::iterator end)
    {
        auto & key = *iter;
        auto style_iter = styles.find(key);
        if (style_iter == styles.end())
            styles[key] = std::make_shared<Style>();
        ++iter;
        if (iter == end)
            return styles[key];

        return styles[key]->get(iter, end);
    }

    std::shared_ptr<Style> Style::get(const std::u8string & name)
    {
        std::vector<std::u8string> ids = core::split(name, u8' ');
        return get(ids.begin(), ids.end());
    }

    void Style::join(Style & another)
    {
        items.merge(another.items);
    }

    void Style::join(const std::map<std::u8string, std::shared_ptr<Style>> & styles)
    {
        for(auto & style : styles)
            items.merge(style.second->items);
    }

    core::error StyleSheet::loadFromFile(const std::u8string & path)
    {
        std::fstream fs;
        fs.open(reinterpret_cast<const std::string &>(path), std::ios::in);
        if (fs)
            return loadFromData(std::u8string(std::istreambuf_iterator<char>(fs), std::istreambuf_iterator<char>()));
        else
            return core::error_io;
    }

    core::error StyleSheet::loadFromData(const std::u8string & data)
    {
        style_string_walker walker(data);
        std::u8string name;
        std::u8string pseudo;
        while (!walker.eof())
        {
            name.clear();
            pseudo.clear();

            walker.readTo('.');
            walker.skip();
            char ch = 0;
            name = walker.readTo("{:", 2, ch);
            if (walker.eof())
                break;

            core::trim(name);
            walker.skip();

            // Î±Àà
            if(ch == ':')
            {
                pseudo = walker.readTo('{');
                core::trim(pseudo);
            }

            if (walker.peek() == '}')
            {
                walker.read();
                continue;
            }

            auto style = get(name);
            while (!walker.eof())
            {
                walker.skip();
                std::u8string key = walker.readTo(':');
                core::trim(key);
                walker.skip();
                char ch = 0;
                std::u8string value = walker.readTo(";}", 2, ch);
                core::trim(value);
                walker.skip();

                if (pseudo.empty())
                    style->items[key] = value;
                else
                    style->pseudos[pseudo][key] = value;
                if (ch == '}')
                    break;

                if (walker.peek() == '}')
                {
                    walker.read();
                    break;
                }
            }
        }
        return core::error_ok;
    }
}
