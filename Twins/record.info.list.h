#pragma once

#if 0
#include "record.info.h"
#include "directory.watch.notify.h"
#include <vector>

namespace Twins
{
    class RecordList
    {
    private:
        typedef std::vector<RecordInfo> ListType;
        
    public:
        typedef ListType::iterator iterator;
        typedef ListType::const_iterator const_iterator;
        typedef ListType::size_type size_type;

        RecordList();
        ~RecordList();

        RecordList(RecordList const& rhs);
        RecordList& operator = (RecordList const& rhs);
        void Swap(RecordList& rhs);

        RecordInfo const& operator[] (int index) const;
        RecordInfo& operator[] (int index);

        int Count() const;
        const_iterator Begin() const;
        const_iterator End() const;
        void PushBack(RecordInfo const& info);
        double Sort(RecordInfo::LessFn lessFn, bool ascending);
        void Clear();
        void Update(PCWSTR dirpath, DirectoryNotifyMap const& nmap, RecordInfo::LessFn const& lessFn, bool ascending);
        const_iterator FindByName(CString const& name, bool isDir) const;

    private:
        ListType records_;

        RecordInfo& Accessor(int index);
        void UpdateItem(PCWSTR dirpath, DirectoryNotifyPtr const& nitem, RecordInfo::LessFn const& lessFn, bool ascending);
        void AddNew(PCWSTR dirpath, DirectoryNotifyPtr const& nitem, RecordInfo::LessFn const& lessFn, bool ascending);
        void RemoveExisting(PCWSTR dirpath, DirectoryNotifyPtr const& nitem);
        void UpdateExisting(PCWSTR dirpath, DirectoryNotifyPtr const& nitem);
    };
}
#endif // 0
