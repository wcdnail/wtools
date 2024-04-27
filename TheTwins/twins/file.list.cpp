#include "stdafx.h"
#include "file.list.h"
#include <todo.fixme.etc.h>
#include <dh.tracing.h>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

namespace Fl
{
    List::~List() {}

    List::List() 
        : Array()
        , DirectoryCounter(0)
        , FileCounter(0)
        , SumFileSize(0)
        , MaximalFileSize(0)
        , MaximalFilenameLen(0)
    {
    }

    List::List(List const& rhs)
        : Array(rhs.Array)
        , DirectoryCounter(rhs.DirectoryCounter)
        , FileCounter(rhs.FileCounter)
        , SumFileSize(rhs.SumFileSize)
        , MaximalFileSize(rhs.MaximalFileSize)
        , MaximalFilenameLen(rhs.MaximalFilenameLen)
    {
    }

    List& List::operator = (List const& rhs)
    {
        List temp(rhs);
        Swap(temp);
        return *this;
    }

    void List::Swap(List& rhs)
    {
        Array.swap(rhs.Array);
        std::swap(DirectoryCounter, rhs.DirectoryCounter);
        std::swap(FileCounter, rhs.FileCounter);
        std::swap(SumFileSize, rhs.SumFileSize);
        std::swap(MaximalFileSize, rhs.MaximalFileSize);
        std::swap(MaximalFilenameLen, rhs.MaximalFilenameLen);
    }

#if 0
    ErrorType List::Load(Fs::path const& root, bool recursive)
    {
        canceled_ = false;

        if (recursive)
            return Load<Fs::recursive_directory_iterator>(root);

        return Load<Fs::directory_iterator>(root);
    }

    template <class Iterator>
    ErrorType List::Load(Fs::path const& root)
    {
        ErrorType ec;
        if (!Fs::exists(root, ec))
            return ec;

        List tempList;

        if (root.parent_path().is_absolute())
            tempList.list_.push_back(Entry::MakeUpperDir());
        
        BOOST_DEDUCED_TYPENAME Iterator dend;
        for (BOOST_DEDUCED_TYPENAME Iterator it(root, ec); it != dend; ++it)
        {
            if (canceled_)
            {
                ec = ErrorType(Sy::posix::operation_canceled, Sy::generic_category());
                break;
            }

            Entry item(*it);
            if (item.IsValid())
                tempList.PushBack(item);
        }

        Swap(tempList);
        return ec;
    }
#endif

    void List::CalcExtraInfo(Entry const& info)
    {
        if (info.IsDir())
            ++DirectoryCounter;
        else
            ++FileCounter;

        MaximalFilenameLen = max(MaximalFilenameLen, (int)info.GetFilename().length());

        SumFileSize += info.GetSize();
        MaximalFileSize = max(MaximalFileSize, info.GetSize());
    }

    void List::Clear() 
    {
        List temp;
        Swap(temp);
    }

    Entry& List::GetByIndex(int index)
    {
        if ((index >= 0) && (index < Count()))
            return Array[(size_type)index];

#pragma message(_TODO("Throw exception..."))
        static Entry dmyEntry;
        return dmyEntry;
    }

    template <class Iter>
    static Iter _FindByName(Iter beg, Iter end, std::wstring const& name)
    {
        return std::find_if(beg, end, boost::bind(&Entry::IsFilenameEq, _1, name));
    }

    List::const_iterator List::FindByName(std::wstring const& name) const
    {
        return _FindByName(Begin(), End(), name);
    }

    void List::Update(wchar_t const* dirpath, Twins::DirectoryNotifyMap const& nmap, LESSFN const& lessfn, bool ascending)
    {
        for(Twins::DirectoryNotifyMap::const_iterator it = nmap.begin(); it != nmap.end(); ++it)
            OnDWNotify(dirpath, it->second, lessfn, ascending);
    }

    void List::OnDWNotify(wchar_t const* dirpath, Twins::DirectoryNotifyPtr const& nitem, LESSFN const& lessfn, bool ascending)
    {
        if (nitem->Added || nitem->RenameDone)
            OnAddNew(dirpath, nitem, lessfn, ascending);

        else if (nitem->Removed || nitem->RenameBegin)
            OnRemoveExisting(nitem);

        else
            OnUpdateExisting(nitem);
    }

    void List::OnAddNew(wchar_t const* dirpath, Twins::DirectoryNotifyPtr const& nitem, LESSFN const& lessfn, bool ascending)
    {
        Entry info(dirpath, *nitem);
        if (info.IsValid())
        {
            CalcExtraInfo(info);

            if (Array.empty() || !lessfn)
                Array.push_back(info);

            else
            {
                const_iterator iafter = std::find_if(Array.begin(), Array.end(), boost::bind(lessfn, info, _1, ascending));
                Array.insert(iafter, info);
            }
        }
    }

    void List::OnRemoveExisting(Twins::DirectoryNotifyPtr const& nitem)
    {
        const_iterator it = _FindByName(Begin(), End(), std::wstring(nitem->Filename));
        if (it != End())
            Array.erase(it);
    }

    void List::OnUpdateExisting(Twins::DirectoryNotifyPtr const& nitem)
    {
        iterator it = _FindByName(Array.begin(), Array.end(), std::wstring(nitem->Filename));
        if (it != End())
            it->Update();
    }

    double List::Sort(LESSFN pf, bool ascending)
    {
        Dh::Timer timer;

        struct SP
        {
            SP(LESSFN pf, bool asc) 
                : lessf(pf)
                , ascnd(asc) 
            {}

            bool operator() (Entry const& L, Entry const& R)
            {
                return lessf(L, R, ascnd);
            }

        private:
            LESSFN lessf;
            bool ascnd;
        };

        std::sort(Array.begin(), Array.end(), SP(pf, ascending));
        return timer.Seconds();
    }

    std::wstring List::ToString(std::wstring const& separator, int count) const
    {
        std::wstring rv;

        for (const_iterator it = Begin(); it != End(); ++it)
        {
            if (!rv.empty())
                rv += separator;

            if (! --count)
            {
                rv += L"...";
                break;
            }
            else
                rv += it->GetFilename();
        }

        return rv;
    }

    void List::PushBack2(std::wstring const& path)
    {
        PushBack(Path(path.c_str()));
    }

    void List::PushBack(Path const& path)
    {
        Entry temp(path.c_str());
        if (temp.IsValid())
            PushBack(temp);
    }

    void List::PushBack(Entry const& entry) 
    {
        CalcExtraInfo(entry);
        Array.push_back(entry); 
    }

    void List::DebugDump() const
    {
#ifdef _DEBUG
        for (EntryArray::const_iterator it = Array.begin(); it != Array.end(); ++it)
            it->DebugDump();
#endif // _DEBUG
    }

    void List::OnEntry(Entry const& entry)
    {
        PushBack(entry);
    }

    void List::OnEntry(Item const& item)
    {
        if (item.IsHidden() && item.Args.SkipHidden)
            return ;

        if (item.IsUpperDir())
            OnEntry(Entry::MakeUpperDir());

        else
        {
            Path path = item.Args.Root.c_str();
            path /= item.Name;

            Entry target;
            target.Update(path, item.Size, item.Flags, item.Time, item.Int);

            if (target.IsValid())
                OnEntry(target);
        }
    }
}
