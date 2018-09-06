#include "stdafx.h"
#include "dll.h"
#include "win32/windows.h"

namespace win32
{
    using namespace core;

    dll::dll() :
        _handle(NULL)
    {

    }

    dll::dll(std::string path) :
        _handle(NULL)
    {
        load(path);
    }

    dll::~dll()
    {
        free();
    }

    core::error dll::load(std::string path)
    {
        free();
        if (!path.empty())
        {
            std::wstring pathw = core::u8str_wstr(path);
            _handle = ::LoadLibraryW(pathw.c_str());
            if (!_handle)
                logger::war() << __FUNCTION__" LoadLibraryW failed " << winerr_str(GetLastError());
        }
        return _handle ? error_ok : error_inner;
    }

    void dll::free()
    {
        if (_handle)
        {
            ::FreeLibrary((HMODULE)_handle);
            _handle = NULL;
        }
    }

    void * dll::proc(std::string name)
    {
        if (_handle)
        {
            std::string namea = core::u8str_astr(name);
            return (void *)::GetProcAddress((HMODULE)_handle, namea.c_str());
        }
        else
            return nullptr;
    }
}
