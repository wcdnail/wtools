#pragma once 

struct GdiPlus
{
    GdiPlus(int requiredver = 1);
    ~GdiPlus();

    static PCSTR StatusString(int status);

private:
    ULONG_PTR Id;
};
