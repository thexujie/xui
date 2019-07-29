#pragma once

#include "player/media.h"

namespace win32
{
    class VideoRenderWindow : public player::IVideoRenderWindow
    {
    public:
        VideoRenderWindow();
        ~VideoRenderWindow();

        void * GetHandle() const { return _hWnd; }

        virtual int64_t WndProc(void * pWnd, uint32_t message, uint64_t wParam, int64_t lParam);
    private:
        void * _hWnd;
    };
}
