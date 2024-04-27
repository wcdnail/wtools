#include "stdafx.h"
#if 0
#include "file.list.sort.h"
#include "file.entry.h"
#ifdef _WIN32
#include <atltime.h>
#endif // _WIN32

namespace Fl
{
    namespace LessImpl
    {
        enum 
        {
            RankUpperDir = 0,
            RankDir,
            RankLast,
        };

        static int Rank(Entry const& info)
        {
            return (info.IsUpperDir() ? RankUpperDir : (info.IsDir() ? RankDir : RankLast));
        }

        static bool ByRank(Entry const& L, Entry const& R)
        {
            return Rank(L) < Rank(R);
        }

        static int CompareNoCase(std::wstring const& r, std::wstring const& l)
        {
            return ::_wcsnicmp(r.c_str(), l.c_str(), l.length());
        }

        static bool ByType(Entry const& L, Entry const& R, bool ascnd)
        {
            int cv = CompareNoCase(L.GetExt(), R.GetExt());
            return (ascnd ? (cv < 0) : (cv > 0));
        }

        static bool BySize(Entry const& L, Entry const& R, bool ascnd)
        {
            return (ascnd ? (L.GetSize() < R.GetSize()) : (L.GetSize() > R.GetSize()));
        }

        static bool ByDate(Entry const& L, Entry const& R, bool ascnd)
        {
#ifdef _WIN32
            CTime Lt(CFileTime(L.GetTime()));
            CTime Rt(CFileTime(R.GetTime()));
#endif // _WIN32
            return (ascnd ? (Lt < Rt) : (Lt > Rt));
        }

        static bool ByName(Entry const& L, Entry const& R, bool ascnd)
        {
            int cr = CompareNoCase(L.GetFilename(), R.GetFilename());
            return (ascnd ? (cr < 0) : (cr > 0));
        }

        template <class Proto>
        static bool Less(Proto Pred, Entry const& L, Entry const& R, bool ascnd, bool dirascnd)
        {
            if (R.IsUpperDir())
                return false;

            if (L.IsDir() && R.IsDir())
                return ByName(L, R, dirascnd);

            if (L.IsDir() || R.IsDir())
                return ByRank(L, R);

            return Pred(L, R, ascnd);
        }
    }

    namespace Sort
    {
        bool ByName(Entry const& L, Entry const& R, bool ascending)
        {
            return LessImpl::Less(LessImpl::ByName, L, R, ascending, ascending);
        }

        bool ByType(Entry const& L, Entry const& R, bool ascending)
        {
            return LessImpl::Less(LessImpl::ByType, L, R, ascending, true);
        }

        bool BySize(Entry const& L, Entry const& R, bool ascending)
        {
            return LessImpl::Less(LessImpl::BySize, L, R, ascending, true);
        }

        bool ByDate(Entry const& L, Entry const& R, bool ascending)
        {
            return LessImpl::Less(LessImpl::ByDate, L, R, ascending, true);
        }

        bool ByShellType(Entry const& L, Entry const& R, bool ascending)
        {
            return false;
        }
    }
}
#endif
