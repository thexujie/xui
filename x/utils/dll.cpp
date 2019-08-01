#include "stdafx.h"
#include "dll.h"
#include "win32/win32.h"

namespace utils
{
    dll::dll() :
        _handle(NULL)
    {

    }

    dll::dll(const std::u8string & path) :
        _handle(NULL)
    {
        load(path);
    }

    dll::~dll()
    {
        release();
    }

	core::error dll::load(const std::u8string & path, bool userDir)
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
				core::war() << __FUNCTION__" LoadLibrary [" << pathw << "] failed " << win32::winerr_str(GetLastError());
            else
            {
                wchar_t temp[1024] = {};
                uint32_t len =GetModuleFileNameW((HMODULE)_handle, temp, 1024);
                _path = core::wstr_u8str(temp, len);
            }
        }
        return _handle ? core::ok : core::e_inner;
    }

    void dll::release()
    {
        if (_handle)
        {
            ::FreeLibrary((HMODULE)_handle);
            _handle = NULL;
        }
    }

	void * dll::proc(const char * name)
    {
		if (_handle)
			return static_cast<void *>(::GetProcAddress((HMODULE)_handle, name));
		else
			return nullptr;
    }

    void * dll::proc(const std::u8string & name)
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
