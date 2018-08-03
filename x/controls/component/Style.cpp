#include "stdafx.h"
#include "Style.h"

namespace controls::component
{
    std::shared_ptr<Style> Style::select(std::vector<std::string>::iterator iter, std::vector<std::string>::iterator end)
    {
        auto style_iter = _styles.find(*iter);
        if (style_iter == _styles.end())
            return nullptr;

        ++iter;
        if (iter == end)
            return style_iter->second;
        return style_iter->second->select(iter, end);
    }

    std::shared_ptr<Style> Style::select(const std::string & name)
    {
        std::vector<std::string> ids = core::string::split(name, ' ');
        return select(ids.begin(), ids.end());
    }

    std::shared_ptr<Style> Style::get(std::vector<std::string>::iterator iter, std::vector<std::string>::iterator end)
    {
        auto & key = *iter;
        auto style_iter = _styles.find(key);
        if (style_iter == _styles.end())
            _styles[key] = std::make_shared<Style>();
        ++iter;
        if (iter == end)
            return _styles[key];

        return _styles[key]->get(iter, end);
    }

    std::shared_ptr<Style> Style::get(const std::string & name)
    {
        std::vector<std::string> ids = core::string::split(name, ' ');
        return get(ids.begin(), ids.end());
    }

    void Style::join(Style & another)
    {
        items.merge(another.items);
    }

    void Style::join(const std::map<std::string, std::shared_ptr<Style>> & styles)
    {
        for(auto & style : styles)
            items.merge(style.second->items);
    }

    core::error StyleSheet::loadFromFile(std::string path)
    {
        return core::error_ok;
    }

    core::error StyleSheet::loadFromData(std::string data)
    {
        // .button { border: 1px;padding : 1em, 0.5em;} .button:hoving{border: 2px;}

        core::string::string_walker walker(data);
        while (!walker.eof())
        {
            walker.readTo('.');
            walker.skip();
            std::string name_str = walker.readTo('{');
            walker.skip();

            if (walker.peek() == '}')
            {
                walker.read();
                continue;
            }

            auto style = get(name_str);
            while (!walker.eof())
            {
                walker.skip();
                std::string key_str = walker.readTo(':');
                walker.skip();
                char ch = 0;
                std::string val_str = walker.readTo(";}", 2, ch);
                walker.skip();

                style->set(key_str, val_str);
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
