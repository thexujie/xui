#include "stdafx.h"
#include "system.h"
#include "path.h"
#include "string.h"

namespace core
{
	pointer_t instance()
	{
		return platform::instance();
	}
	
    core::filesystem::path temp_path()
    {
		return platform::temp_path();
    }

    core::filesystem::path appdata_path()
    {
		return platform::appdata_path();
    }

    core::filesystem::path process_path()
    {
		return platform::process_path();
    }

    core::filesystem::path current_path()
    {
		return platform::current_path();
    }

    std::string process_name()
    {
        return process_path().filename().string();
    }

	uint32_t thread_id()
    {
		return platform::thread_id();
    }

	uint32_t process_id()
    {
		return platform::process_id();
    }

	void set_thread_name(uint32_t thread_id, std::string name)
    {
		platform::set_thread_name(thread_id, name);
    }

	void set_thread_priority(thread_priority priority)
    {
		platform::set_thread_priority(priority);
    }
}
