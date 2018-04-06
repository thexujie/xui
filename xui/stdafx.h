#pragma once

#define NOMINMAX

#include <stdio.h>
#include <tchar.h>
#include <stdint.h>
#include <assert.h>

#include <cctype>
#include <locale>
#include <string>
#include <variant>
#include <any>

#include <array>
#include <vector>
#include <map>
#include <list>
#include <memory>
#include <map>
#include <queue>

#include <sstream>
#include <fstream>
#include <iostream>

#include <numeric>
#include <functional>
#include <algorithm> 
#include <regex>
#include <random>

#include <mutex>
#include <atomic>
#include <future>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;


#define USE_UTF8
typedef std::string a8string;
typedef std::string u8string;
typedef std::wstring u16string;
typedef char byte_t;

#include "core/error.h"
#include "core/system.h"
#include "core/logger.h"
#include "core/path.h"
