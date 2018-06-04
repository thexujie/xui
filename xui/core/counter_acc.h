#pragma once

namespace core
{
    template<typename ValT, int32_t count>
    class counter_acc
    {
    public:
        counter_acc(std::chrono::microseconds period = 1s) : _period(period)
        {
            for (auto & val : _vals)
                val = 0;
        }

        void acc(ValT val)
        {
            std::chrono::steady_clock::time_point tp_now = std::chrono::high_resolution_clock::now();
            if (tp_now - _tp_last > _period)
            {
                _vals[_index++ % count] = _val;
                _val = 0;
                _tp_last = tp_now;
            }
            _val += val;
        }
        ValT avg() const
        {
            return std::accumulate(_vals.begin(), _vals.end(), (ValT)0) / _vals.size();
        }


    private:
        std::chrono::microseconds _period;
        std::chrono::steady_clock::time_point _tp_last;
        std::array<ValT, count> _vals;

        int64_t _index = 0;
        ValT _val = 0;
    };
}