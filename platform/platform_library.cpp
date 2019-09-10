#include "stdafx.h"
#include "platform.h"

namespace core
{
    library::library() :
        _handle(NULL)
    {

    }

    library::library(const std::u8string & path) :
        _handle(NULL)
    {
        load(path);
    }

    library::~library()
    {
        release();
    }

	core::error library::load(const std::u8string & path, bool userDir)
    {
        release();
        if (!path.empty())
        {
            std::wstring pathw = core::u8str_wstr(path);
            if(userDir)
                _handle = ::LoadLibraryExW(pathw.c_str(), NULL, LOAD_LIBRARY_SEARCH_USER_DIRS);
            else
                _handle = ::LoadLibraryW(pathw.c_str());
			if (!_handle)
				core::war() << __FUNCTION__" LoadLibrary [" << pathw << "] failed " << platform::win32::winerr_str(GetLastError());
            else
            {
                wchar_t temp[1024] = {};
                uint32_t len =GetModuleFileNameW((HMODULE)_handle, temp, 1024);
                _path = core::wstr_u8str(temp, len);
            }
        }
        return _handle ? core::ok : core::e_inner;
    }

    void library::release()
    {
        if (_handle)
        {
            ::FreeLibrary((HMODULE)_handle);
            _handle = NULL;
        }
    }

	void * library::proc(const char * name)
    {
		if (_handle)
			return static_cast<void *>(::GetProcAddress((HMODULE)_handle, name));
		else
			return nullptr;
    }

    void * library::proc(const std::u8string & name)
    {
        if (_handle)
        {
            std::string namea = core::u8str_astr(name);
            return static_cast<void *>(::GetProcAddress((HMODULE)_handle, namea.c_str()));
        }
        else
            return nullptr;
    }
}
