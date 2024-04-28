#pragma once

#include "basic.entry.h"
#include <string>
#include <cstdint>

#ifdef _WIN32
#include <atltime.h>
#endif

namespace Twins
{
    class Enumerator
    {
    public:
        typedef std::error_code Ec;

        Enumerator();
        virtual ~Enumerator();

        struct LoadArgs
        {
            std::wstring Root;
            std::wstring Mask;
            bool SkipHidden;
            bool SkipUpper;
            bool Recursive;
            unsigned Deep;

            LoadArgs(std::wstring const& root, std::wstring const& mask, bool nohidden, bool noupper, bool recursive);
        };

        struct Item
        {
            LoadArgs const& Args;
            std::wstring Name;
            uint64_t Size;
            uint64_t Time;
            unsigned Flags;
            unsigned Int;

#ifdef _WIN32
            Item(WIN32_FIND_DATAW const& data, LoadArgs const& params);

            ATL::CTime GetAtlTime() const;
#endif
            bool IsDir() const;
            bool IsUpperDir() const;
            bool IsCurrentDir() const;
            bool IsHidden() const;
        };

        void Reset();
        virtual Ec Load(LoadArgs const& params);
        void Cancel();
        bool Canceled() const;

    protected:
        virtual void OnEntry(Item const& item);

    private:
        volatile bool Stop;

    private:
        Enumerator(Enumerator const&);
        Enumerator& operator = (Enumerator const&);
    };
}
