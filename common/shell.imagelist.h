#pragma once

#include "wcdafx.api.h"
#include <map>
#include <windef.h>
#include <commctrl.h>

namespace Sh
{
    class Imagelist
    {
    public:
        DELETE_COPY_MOVE_OF(Imagelist);

        WCDAFX_API Imagelist(bool useSmallIcons); /* throw(std::runtime_error) */
        WCDAFX_API ~Imagelist();

        WCDAFX_API HIMAGELIST Get() const;

    private:
        typedef std::map<int, HICON> IconMap;

        HIMAGELIST Handle;
        IconMap Icons;

        void Clear();
    };

    inline HIMAGELIST Imagelist::Get() const { return Handle; }
}
