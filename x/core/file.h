#pragma once

namespace core
{
    class file
    {
    public:
		file() = delete;

		static std::vector<byte_t> read_full(std::u8string_view path);
		static std::string read_full_string(std::u8string_view path);
		static std::u8string read_full_u8string(std::u8string_view path);
    };
}
