#pragma once

namespace win32
{
    namespace sync
    {
        class event
        {
        public:
            event();
            event(bool manual, bool set);

            void set();
            void reset();

            void * handle() const { return _handle; }
        private:
            void * _handle;
        };
    }
}
