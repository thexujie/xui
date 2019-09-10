#pragma once

#include "path.h"

namespace core
{
	pointer_t instance();
    core::filesystem::path temp_path();
    core::filesystem::path appdata_path();
    core::filesystem::path process_path();
    core::filesystem::path current_path();
    std::string process_name();
    uint32_t thread_id();
	uint32_t process_id();

	void set_thread_name(uint32_t thread_id, std::string name);

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

	void set_thread_priority(thread_priority priority);
}
