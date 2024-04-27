#pragma once

#include <boost/system/error_code.hpp>
#include <filesystem>

namespace Twins
{
    class Entry
    {
    public:
        enum 
        {
            Normal              = 0x00000000,
            Directory           = 0x00000001,
            CurrentDirectory    = 0x00000002,
            UpperDirectory      = 0x00000004,
            Symlink             = 0x00000008,
            Reparse             = 0x00000010,
            Hidden              = 0x00000020,
        };

        virtual ~Entry();

    protected:
        Entry();

    private:
        Entry(Entry const&);
        Entry& operator = (Entry const&);
    };
}
