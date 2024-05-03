#pragma once 

namespace Initialize
{
    struct GdiPlus
    {
        GdiPlus(int requiredver = 1);
        ~GdiPlus();

        static char const* StatusString(int status);

    private:
        ULONG_PTR Id;
    };
}
