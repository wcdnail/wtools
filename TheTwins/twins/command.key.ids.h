#pragma once

#include <winuser.h>

namespace Twins
{
    namespace Command
    {
        namespace KeyId
        {
            enum Flags
            {
                None        = 0,
                FlagMask    = 0xff000000,
                KeyMaks     = 0x00ffffff,
                Ctrl        = 0x01000000,
                Alt         = 0x02000000,
                Shift       = 0x04000000,
            };

            #define _KeyId(Code, Flags) \
                (((unsigned)(Flags)) & FlagMask) | (((unsigned)(Code)) & KeyMaks)

            enum Ids
            {
                Dummy           = 0xffffffff,
                No              = 0xfffffebe,
                Alt_F1          = _KeyId(VK_F1, Alt),
                Alt_F2          = _KeyId(VK_F2, Alt),
                Alt_Enter       = _KeyId(VK_RETURN, Alt),
                Tab             = _KeyId(VK_TAB, None),
                Enter           = _KeyId(VK_RETURN, None),
                Escape          = _KeyId(VK_ESCAPE, None),
                Delete          = _KeyId(VK_DELETE, None),
                F1              = _KeyId(VK_F1, None),
                F2              = _KeyId(VK_F2, None),
                F3              = _KeyId(VK_F3, None),
                F4              = _KeyId(VK_F4, None),
                Shift_F4        = _KeyId(VK_F4, Shift),
                F5              = _KeyId(VK_F5, None),
                F6              = _KeyId(VK_F6, None),
                F7              = _KeyId(VK_F7, None),
                F8              = _KeyId(VK_F8, None),
                F9              = _KeyId(VK_F9, None),
                F10             = _KeyId(VK_F10, None),
                F11             = _KeyId(VK_F11, None),
                F12             = _KeyId(VK_F12, None),
                Del             = _KeyId(VK_DELETE, None),
                Shift_Del       = _KeyId(VK_DELETE, Shift),
                Ctrl_H          = _KeyId('H', Ctrl),
                Alt_Up          = _KeyId(VK_UP, Alt),
                Alt_Down        = _KeyId(VK_DOWN, Alt),
                Ctrl_Up         = _KeyId(VK_UP, Ctrl),
                Ctrl_Down       = _KeyId(VK_DOWN, Ctrl),
                Multiply        = _KeyId(VK_MULTIPLY, None), 
                Plus            = _KeyId(VK_ADD, None),     
                Minus           = _KeyId(VK_SUBTRACT, None),    
                Insert          = _KeyId(VK_INSERT, None),   
                Up              = _KeyId(VK_UP, None),       
                Down            = _KeyId(VK_DOWN, None),
                Shift_Up        = _KeyId(VK_UP, Shift),  
                Shift_Down      = _KeyId(VK_DOWN, Shift),
                Home            = _KeyId(VK_HOME, None),     
                End             = _KeyId(VK_END, None),      
                PageUp          = _KeyId(VK_PRIOR, None),   
                PageDown        = _KeyId(VK_NEXT, None), 
                Right           = _KeyId(VK_RIGHT, None), 
                Alt_F8          = _KeyId(VK_F8, Alt), 
                Ctrl_P          = _KeyId('P', Ctrl), 
                Ctrl_Enter      = _KeyId(VK_RETURN, Ctrl), 
                Alt_F7          = _KeyId(VK_F7, Alt), 
                Space           = _KeyId(VK_SPACE, None), 
                AltShiftEnter   = _KeyId(VK_RETURN, Alt | Shift), 
                Alt_X           = _KeyId('X', Alt), 
            };

            inline unsigned MakeId(unsigned code, unsigned flags) { return _KeyId(code, flags); }

            inline bool IsShift(unsigned code) { return 0 != (code & Shift); }
            inline bool IsAlt(unsigned code) { return 0 != (code & Alt); }
            inline bool IsCtrl(unsigned code) { return 0 != (code & Ctrl); }

            #undef _KeyId
        };
    }
}
