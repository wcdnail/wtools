#pragma once 

struct ScopedInitGdiplus
{
    ScopedInitGdiplus(int requiredver = 1);
    ~ScopedInitGdiplus();

    static PCSTR StatusString(int status);

private:
    ULONG_PTR Id;
};
