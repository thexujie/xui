#pragma once

namespace controls::component
{
    class Animation
    {
    public:
        Animation() = default;
        ~Animation() = default;

        void clear() { animations.clear(); }
        void add(std::shared_ptr<core::animation> animation) { animations.push_back(animation); }
        void start();
        void update();

    private:
        std::vector<std::shared_ptr<core::animation>> animations;
    };
}
