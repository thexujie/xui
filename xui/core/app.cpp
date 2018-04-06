#include "stdafx.h"
#include "app.h"

namespace core
{
    thread_local IApp * __app = nullptr;

    void app(IApp * ptr)
    {
        __app = ptr;
    }

    IApp & app()
    {
        return *__app;
    }
}
