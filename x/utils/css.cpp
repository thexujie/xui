#include "stdafx.h"
#include "css.h"

namespace utils
{
    CSS::CSS()
    {
        
    }

    CSS::~CSS()
    {
        
    }

    core::error CSS::loadFromData(std::string data)
    {
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

            while (!walker.eof())
            {
                walker.skip();
                std::string key_str = walker.readTo(':');
                walker.skip();
                char ch = 0;
                std::string val_str = walker.readTo(";}", 2, ch);
                walker.skip();
                _items[name_str][key_str] = val_str;

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