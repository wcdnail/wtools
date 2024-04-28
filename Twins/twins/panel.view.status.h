#pragma once

#include <boost/noncopyable.hpp>
#include <string.hp.h>
#include <atlctrls.h>

namespace Fl
{
    class Entry;
    class List;
}

namespace Twins
{
    class SelectHelper;
    struct FScanner;

    class ViewStatus: boost::noncopyable
    {
    public:
        typedef unsigned __int64 SizeType;

        ViewStatus();
        ~ViewStatus();

        void Clear(HWND owner, bool invalidate);
        void Reset(FScanner const& scanner, HWND owner = NULL, bool invalidate = FALSE);
        void Update(FScanner const& scanner, HWND owner = NULL, bool invalidate = FALSE);

        void Count(Fl::List const& entries, Fl::Entry const& info, bool add);
        void Count(Fl::List const& entries, SelectHelper const& selector, HWND owner, bool invalidate);

        void OnPaint(CDC& dc, CRect const& rc) const;

    private:
        friend class SelectHelper;

        WidecharString Text;
        int SelFiles;
        int SelDirs;
        SizeType SelSize;

        // Appearance
        COLORREF MyBackColor;
        COLORREF MyTextColor;
    };
}
