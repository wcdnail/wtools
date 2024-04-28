#pragma once

#include <string>

namespace Twins
{
    namespace Keyboard
    {
        wchar_t const* CodeString(unsigned code);

        struct KeyState
        {
            bool Shift;
            bool Alt;
            bool Ctrl;
            unsigned Code;
            unsigned Flags;

            KeyState(unsigned code, unsigned flags, bool shift, bool alt, bool ctrl);
            KeyState(unsigned code = 0, unsigned flags = 0);
            ~KeyState();

            void Dump() const;
            unsigned KeyId() const;
            std::wstring KeyIdName() const;
        };
    }
}
