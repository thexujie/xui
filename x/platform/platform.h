#pragma once

namespace platform
{
    int runLoop();
	pointer_t instance();
    void endLoop(int32_t ret);
	uint32_t process_id();
	uint32_t thread_id();

	core::filesystem::path temp_path();
	core::filesystem::path appdata_path();
	core::filesystem::path process_path();
	core::filesystem::path current_path();

	void set_thread_name(uint32_t thread_id, std::string name);
	void set_thread_priority(core::thread_priority priority);

	
	namespace win32
	{
		void debug_output(const std::u8string & str);
	}
}
