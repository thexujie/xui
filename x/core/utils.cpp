#include "stdafx.h"
#include "utils.h"

namespace core
{
	void debug_output(std::u8string str)
	{
		return platform::win32::debug_output(str);
	}
}
