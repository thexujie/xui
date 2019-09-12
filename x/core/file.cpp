#include "stdafx.h"
#include "file.h"

namespace core
{
	std::vector<byte_t> file::read_full(std::u8string_view path)
	{
		std::fstream fs;
		fs.open(core::u8str_wstr(path), std::ios::in | std::ios::binary);
		if (!fs.good())
			return {};

		fs.seekg(0, std::ios::end);
		auto size = fs.tellg();
		fs.seekg(0, std::ios::beg);

		std::vector<byte_t> data;
		data.reserve(size);
		data.resize(size);
		fs.read(data.data(), size);
		if (fs.tellg() != size)
			return {};
		return data;
	}

	std::string file::read_full_string(std::u8string_view path)
	{
		std::fstream fs;
		fs.open(core::u8str_wstr(path), std::ios::in | std::ios::binary);
		if (!fs.good())
			return {};

		fs.seekg(0, std::ios::end);
		auto size = fs.tellg();
		fs.seekg(0, std::ios::beg);

		std::string data;
		data.reserve(size);
		data.resize(size);
		fs.read(data.data(), size);
		if (fs.tellg() != size)
			return {};
		return data;
	}

	std::u8string file::read_full_u8string(std::u8string_view path)
	{
		std::u8fstream fs;
		fs.open(core::u8str_wstr(path), std::ios::in | std::ios::binary);
		if (!fs.good())
			return {};

		fs.seekg(0, std::ios::end);
		auto size = fs.tellg();
		fs.seekg(0, std::ios::beg);

		std::u8string data;
		data.reserve(size);
		data.resize(size);
		fs.read(data.data(), size);
		if (fs.tellg() != size)
			return {};
		return data;
	}
}
