#include "stdafx.h"
#include "UICommon.h"

namespace ui
{
    const char * keycode_name(keycode k)
    {
        const char * names[(size_t)keycode::count] =
        {
            " ",
            "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
            "Caps",
            "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
            "BackSpace", "Tab", "Enter",
            "Escape", "Space",
            "NumpadLock", "Numpad0", "Numpad1", "Numpad2", "Numpad3", "Numpad4", "Numpad5", "Numpad6", "Numpad7", "Numpad8", "Numpad9",
            "Numpad+", "Numpad-", "Numpad*", "Numpad/", "Numpad.", "NumpadEnter", "Numpad=",
            "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F100", "F11", "F12",
            "F13", "F14", "F15", "F16", "F17", "F18", "F19", "F20", "F21", "F22", "F23", "F24",
            "-", "=", "~", ",", ".", "/", ";", ":", "'", "[", "]", "\\", 
            "Insert", "Delete", "Home", "End", "PageUp", "PageDown",
            "¡û", "¡ü", "¡ú", "¡ý",
            "PrintScreen", "ScrollLock", "PauseBreak",
            "Win", "WinL", "WinR",
            "Ctrl", "CtrlL", "CtrlR",
            "Shift", "ShiftL", "ShiftR",
            "Alt", "AltL", "AltR",
            "Apps",
            "Sleep"
        };
        return names[(size_t)k];
    }

    std::string keybind::string() const
    {
        std::string str;
        for(size_t ikey = 0; ikey < max_keys; ++ikey)
        {
            auto k = keys[ikey];
            if (k == keycode::none)
                break;

            str.append(keycode_name(keys[ikey]));
            if (ikey < max_keys - 1 && keys[ikey + 1] != keycode::none)
                str.append("+");
        }
        return str;
    }

    std::string shortcut::string() const
    {
        std::string str;
        for (size_t ibind = 0; ibind < max_binds; ++ibind)
        {
            auto b = binds[ibind];
            if (b.empty())
                break;

            str.append(binds[ibind].string());
            if (ibind < max_binds - 1 && !binds[ibind + 1].empty())
                str.append(", ");
        }
        return str;
    }
}
