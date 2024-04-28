#include "stdafx.h"
#include "cdefines.h"

#if defined(_MSC_VER) && defined(_M_IX86)
_extrnc void _chkstk();
_extrnc void _alloca_probe_8();

_extrnc __declspec(naked) void _ccdecl _alloca_probe_16()
{
    __asm 
    {
        // 16 byte aligned alloca
        push    ecx
        lea     ecx, [esp] + 8  // TOS before entering this function
        sub     ecx, eax        // New TOS
        and     ecx, (16 - 1)   // Distance from 16 bit align (align down)
        add     eax, ecx        // Increase allocation size
        sbb     ecx, ecx        // ecx = 0xFFFFFFFF if size wrapped around
        or      eax, ecx        // cap allocation size on wraparound
        pop     ecx             // Restore ecx
        jmp     _chkstk
    }
}

_extrnc __declspec(naked) void _ccdecl _alloca_probe_8()
{
    __asm 
    {
        // 8 byte aligned alloca
        push    ecx
        lea     ecx, [esp] + 8  // TOS before entering this function
        sub     ecx, eax        // New TOS
        and     ecx, (8 - 1)    // Distance from 8 bit align (align down)
        add     eax, ecx        // Increase allocation Size
        sbb     ecx, ecx        // ecx = 0xFFFFFFFF if size wrapped around
        or      eax, ecx        // cap allocation size on wraparound
        pop     ecx             // Restore ecx
        jmp     _chkstk
    }
}
#
#endif
