#pragma once

#include "wcdafx.api.h"
#include <map>
#include <boost/noncopyable.hpp>
#include <windef.h>
#include <commctrl.h>

namespace Sh
{
    class Imagelist: boost::noncopyable
    {
    public:
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
