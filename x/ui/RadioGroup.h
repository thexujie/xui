#pragma once
#include "base/Radio.h"

namespace ui
{
    namespace base
    {
        class Radio;
    }

    class RadioGroup
    {
    public:
        RadioGroup(std::string name) : _name(name) {}

        void check(std::shared_ptr<base::Radio> radio);
        void addRadio(std::shared_ptr<base::Radio> radio);
        void removeRadio(std::shared_ptr<base::Radio> radio);
        const std::vector< std::shared_ptr<base::Radio>> & radios() const { return _radios; }

    private:
        std::string _name;
        std::vector<std::shared_ptr<base::Radio>> _radios;
    };
}
