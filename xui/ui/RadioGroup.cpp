#include "stdafx.h"
#include "RadioGroup.h"
#include "base/Radio.h"

namespace ui
{
    void RadioGroup::check(std::shared_ptr<base::Radio> radio)
    {
        for (auto & wr : _radios)
        {
			auto r = wr.lock();
            if (r != radio)
                r->setCheckState(check_state::unchecked);
        }
    }

    void RadioGroup::addRadio(std::shared_ptr<base::Radio> radio)
    {
        auto iter = std::find_if(_radios.begin(), _radios.end(), [&](auto & wr) { return wr.lock() == radio;});
        if (iter != _radios.end())
            return;

        _radios.push_back(radio);
    }

    void RadioGroup::removeRadio(std::shared_ptr<base::Radio> radio)
    {
        auto iter = std::find_if(_radios.begin(), _radios.end(), [&](auto & wr) { return wr.lock() == radio; });
        if (iter == _radios.end())
            return;

        _radios.erase(iter);
    }

}
