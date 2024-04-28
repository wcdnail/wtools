#if defined(_MSC_VER) && defined(_M_IX86)
#
extern "C" {

int _sse2_avialable = 1;

void __cdecl ftol2();
void __cdecl _ftol2_pentium4();

__declspec(naked) void __cdecl ftol()
{
    __asm jmp ftol2;
}

__declspec(naked) void __cdecl ftol2()
{
    __asm
    {
        ret
    }
}

__declspec(naked) void __cdecl _ftol2_sse()
{
    if (_sse2_avialable)
        __asm jmp _ftol2_pentium4;

    __asm jmp ftol2;
}

__declspec(naked) void __cdecl _ftol2_pentium4()
{
    __asm
    {
        push        ebp
        mov         ebp, esp
        sub         esp, 8
        and         esp, 0x0fffffff8
        fstp        qword ptr [esp]
        cvttsd2si   eax, [esp]
        leave
        retn
    }
}

}
#
#endif
