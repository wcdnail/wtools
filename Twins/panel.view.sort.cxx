#include "stdafx.h"
#include "panel.view.sort.h"
#include <dh.timer.h>
#include <dh.tracing.h>

namespace Twins
{
    FSort::~FSort() {}

    FSort::FSort()
        : Index(NULL)
    {}

    int FSort::GetIndex(int n) const
    {
        if ((n < 0) || (n > (int)Index.size() - 1))
            return n;

        return Index[n];
    }

    namespace Sort
    {
        struct Helper
        {
            enum 
            {
                RankUpperDir = 0,
                RankDir,
                RankLast,
            };

            FItemVector const& Items;
            int Column;
            bool Asc;
            
            Helper(FItemVector const& items, int column, bool asc) 
                : Items(items)
                , Column(column)
                , Asc(asc)
            {}

            static int Rank(FItem const* info) { return (info->IsUpperDir() ? RankUpperDir : (info->IsDir() ? RankDir : RankLast)); }
            static bool ByRank(FItem const* L, FItem const* R) { return Rank(L) < Rank(R); }

            static int CompareNoCase(wchar_t const* ls, wchar_t const* rs, size_t maxc) 
            { 
                return ::_wcsnicmp(ls, rs, maxc); 
            }

            static bool ByType(FItem const* L, FItem const* R, bool ascnd)
            {
                size_t ln = 0, rn = 0;

                wchar_t const* le = L->GetExt(ln);
                wchar_t const* re = R->GetExt(rn);

                int cv = CompareNoCase(le, re, max(ln, rn));
                return (ascnd ? (cv < 0) : (cv > 0));
            }

            static bool BySize(FItem const* L, FItem const* R, bool ascnd)
            {
                return (ascnd ? (L->GetSize() < R->GetSize()) : (L->GetSize() > R->GetSize()));
            }

            static bool ByDate(FItem const* L, FItem const* R, bool ascnd)
            {
#ifdef _WIN32
                LONG rv = ::CompareFileTime((FILETIME const*)&L->LastAccessTime, (FILETIME const*)&R->LastAccessTime);
#endif
                return (ascnd ? (rv < 0) : (rv > 0));
            }

            static bool ByName(FItem const* L, FItem const* R, bool ascnd)
            {
                size_t ln = 0, rn = 0;

                wchar_t const* lname = L->GetFilename(ln);
                wchar_t const* rname = R->GetFilename(rn);

                int cr = CompareNoCase(lname, rname, max(ln, rn));

                return (ascnd ? (cr < 0) : (cr > 0));
            }

            template <class Proto>
            static bool Less(Proto Pred, FItem const* L, FItem const* R, bool ascnd, bool dirascnd)
            {
                if (R->IsUpperDir())
                    return false;

                if (L->IsDir() && R->IsDir())
                    return ByName(L, R, dirascnd);

                if (L->IsDir() || R->IsDir())
                    return ByRank(L, R);

                return Pred(L, R, ascnd);
            }

            static bool ByNameProxy(FItem const* L, FItem const* R, bool a) { return Less(ByName, L, R, a, a); }
            static bool ByTypeProxy(FItem const* L, FItem const* R, bool a) { return Less(ByType, L, R, a, true); }
            static bool BySizeProxy(FItem const* L, FItem const* R, bool a) { return Less(BySize, L, R, a, true); }
            static bool ByDateProxy(FItem const* L, FItem const* R, bool a) { return Less(ByDate, L, R, a, true); }
        
            bool operator() (int l, int r) const;
        };

        bool Helper::operator() (int l, int r) const
        {
            typedef bool (*CompFn)(FItem const*, FItem const*, bool);

            static const CompFn compfn[] = 
            {
              ByNameProxy
            , ByTypeProxy
            , BySizeProxy
            , ByDateProxy
            , ByNameProxy // ByAttributes
            , ByTypeProxy // ByShellType
            };

            if (Column < 0 || Column > _countof(compfn))
                return false;

            return compfn[Column](Items[l], Items[r], Asc);
        }
    }

    void FSort::CreateIndex(int column, bool ascending, FItemVector const& items)
    {
        DH::Timer tm;

        size_t count = items.size();
        IndexVector temp;
        temp.reserve(count);

        for (int i = 0; i < static_cast<int>(count); i++) {
            temp.push_back(i);
        }
        IndexVector::iterator end = temp.end();
        IndexVector::iterator beg = temp.begin();
        if (!items.empty() && items[0]->IsUpperDir())
            ++beg;

        std::sort(beg, end, Sort::Helper(items, column, ascending));
        Index.swap(temp);

        DH::TPrintf(L"SORTING", L"%d %2.8f sec.\n", static_cast<int>(count), tm.Seconds());
    }

    int FSort::FindIndexByName(FItemVector const& items, std::wstring const& name) const
    {
        DH::Timer tm;

        struct pred
        {
            pred(FItemVector const& items, std::wstring const& name)
                : Items(items)
                , Name(name) 
            {}

            bool operator() (int n) const
            {
                return 0 == Name.compare(0, Name.length(), Items[n]->FileName, Items[n]->FileNameLength);
            }

        private:
            FItemVector const& Items;
            std::wstring const& Name;
        };

        IndexVector::const_iterator it = std::find_if(Index.begin(), Index.end(), pred(items, name));
        int rv = (it == Index.end() ? -1 : (int)std::distance(Index.begin(), it));

        DH::TPrintf(L"FINDBYNM", L"%d %2.8f sec.\n", (int)items.size(), tm.Seconds());

        return rv;
    }
}
