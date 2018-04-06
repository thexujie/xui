#pragma once

#include "path.h"

namespace core
{
    core::filesystem::path temp_path();
    core::filesystem::path appdata_path();
    core::filesystem::path process_path();
    std::string process_name();
    int32_t thread_id();
    int32_t process_id();

    void thread_set_name(int thread_id, std::string name);
    enum thread_priority
    {
        thread_priority_idle,
        thread_priority_lowest,
        thread_priority_low,
        thread_priority_normal,
        thread_priority_high,
        thread_priority_highest,
        thread_priority_realtime,
    };
    void thread_set_priority(thread_priority priority);
}
