#include "stdafx.h"
#if 0
#include "record.info.list.h"
#include "fs.enum.h"
#include <dev.assist/dev.assist.h>
#include <algorithm>

namespace Twins
{
    RecordList::RecordList() 
        : records_()
    {}

    RecordList::~RecordList() 
    {}

    RecordList::RecordList(RecordList const& rhs)
        : records_(rhs.records_)
    {
    }

    RecordList& RecordList::operator = (RecordList const& rhs)
    {
        RecordList temp(rhs);
        Swap(temp);
        return *this;
    }

    void RecordList::Swap(RecordList& rhs)
    {
        records_.swap(rhs.records_);
    }

    int RecordList::Count() const { return (int)records_.size(); }
    void RecordList::PushBack(RecordInfo const& info) { records_.push_back(info); }
    RecordList::const_iterator RecordList::Begin() const { return records_.begin(); }
    RecordList::const_iterator RecordList::End() const { return records_.end(); }
    RecordInfo const& RecordList::operator[] (int index) const { return const_cast<RecordList*>(this)->Accessor(index); }
    RecordInfo& RecordList::operator[] (int index) { return Accessor(index); }

    RecordInfo& RecordList::Accessor(int index)
    {
        if ((index >= 0) && (index < Count()))
            return records_[(ListType::size_type)index];

        // TODO: throw exception...
        static RecordInfo dummy;
        return dummy;
    }

    double RecordList::Sort(RecordInfo::LessFn lessFn, bool ascending)
    {
        Dh::ScopeTimer timer;

        struct SortPred
        {
            SortPred(RecordInfo::LessFn lessFn, bool ascending)
                : lessFn_(lessFn)
                , ascending_(ascending)
            {}

            bool operator() (RecordInfo const& L, RecordInfo const& R)
            {
                return lessFn_(L, R, ascending_);
            }

        private:
            RecordInfo::LessFn lessFn_;
            bool ascending_;
        };

        std::sort(records_.begin(), records_.end(), SortPred(lessFn, ascending));
        return timer.Seconds();
    }

    void RecordList::Clear()
    {
        records_.clear();
    }

    RecordList::const_iterator RecordList::FindByName(CString const& name, bool isDir) const
    {
        const_iterator it;

        if (isDir)
            it = std::find_if(records_.begin(), records_.end(), boost::bind(RecordInfo::SameName, _1, name));

        else
        {
            CString n, e;
            Fs::Split(name, isDir, n, e);
            it = std::find_if(records_.begin(), records_.end(), boost::bind(RecordInfo::SameNameAndType, _1, n, e));
        }

        return it;
    }

    void RecordList::Update(PCWSTR dirpath, DirectoryNotifyMap const& nmap, RecordInfo::LessFn const& lessFn, bool ascending)
    {
        for(DirectoryNotifyMap::const_iterator it = nmap.begin(); it != nmap.end(); ++it)
            UpdateItem(dirpath, it->second, lessFn, ascending);
    }

    void RecordList::UpdateItem(PCWSTR dirpath, DirectoryNotifyPtr const& nitem, RecordInfo::LessFn const& lessFn, bool ascending)
    {
        if (nitem->Added || nitem->RenameDone)
            AddNew(dirpath, nitem, lessFn, ascending);

        else if (nitem->Removed || nitem->RenameBegin)
            RemoveExisting(dirpath, nitem);

        else
            UpdateExisting(dirpath, nitem);
    }

    void RecordList::AddNew(PCWSTR dirpath, DirectoryNotifyPtr const& nitem, RecordInfo::LessFn const& lessFn, bool ascending)
    {
        RecordInfo info(dirpath, *nitem);
        if (info.IsValid())
        {
            if (records_.empty() || !lessFn)
                records_.push_back(info);

            else
            {
                const_iterator insertAfter = std::find_if(records_.begin(), records_.end(), boost::bind(lessFn, info, _1, ascending));
                records_.insert(insertAfter, info);
            }
        }
    }

    void RecordList::RemoveExisting(PCWSTR dirpath, DirectoryNotifyPtr const& nitem)
    {
        const_iterator it = FindByName(nitem->Filename, Fs::IsDir(dirpath, nitem->Filename));

        if (it != End())
            records_.erase(it);
    }

    void RecordList::UpdateExisting(PCWSTR dirpath, DirectoryNotifyPtr const& nitem)
    {
        const_iterator it = FindByName(nitem->Filename, Fs::IsDir(dirpath, nitem->Filename));

        if (it != End())
            Fs::UpdateEntry((RecordInfo&)(*it));
    }
}
#endif // 0
