#pragma once

#include "file.entry.h"
#include "basic.enumerator.h"

namespace Fl
{
    class List: Twins::Enumerator
    {
    public:
        typedef Twins::Enumerator Super;
        typedef EntryArray::iterator iterator;
        typedef EntryArray::const_iterator const_iterator;
        typedef EntryArray::size_type size_type;
        typedef bool (*LESSFN)(Entry const&, Entry const&, bool);

        virtual ~List();
        List();
        List(List const& rhs);
        List& operator = (List const& rhs);
        void Swap(List& rhs);

        using Super::Reset;
        using Super::Load;
        using Super::Cancel;
        using Super::Canceled;

        const_iterator Begin() const;
        const_iterator End() const;
        Entry const& operator[] (int index) const;
        Entry& operator[] (int index);
        void DebugDump() const;
        int Count() const;
        void Clear();
        const_iterator FindByName(std::wstring const& name) const;
        void Update(wchar_t const* dirpath, Twins::DirectoryNotifyMap const& nmap, LESSFN const& lessfn, bool ascending);
        void PushBack(Entry const& entry);
        void PushBack(Path const& path);
        void PushBack2(std::wstring const& path);
        double Sort(LESSFN pf, bool ascending);
        std::wstring ToString(std::wstring const& separator, int count) const;
        void Reserve(size_type count);
        unsigned DirCount() const;
        unsigned FileCount() const;
        uint64_t SumSize() const;
        uint64_t MaxFileSize() const;
        int MaxFilenameLen() const;

    protected:
        virtual void OnEntry(Item const& params);
        virtual void OnEntry(Entry const& entry);

    private:
        EntryArray Array;
        unsigned DirectoryCounter;
        unsigned FileCounter;
        uint64_t SumFileSize;
        uint64_t MaximalFileSize;
        int MaximalFilenameLen;

        void CalcExtraInfo(Entry const& ent);
        Entry& GetByIndex(int index);

        void OnDWNotify(wchar_t const* dirpath, Twins::DirectoryNotifyPtr const& nitem, LESSFN const& lessfn, bool ascending);
        void OnAddNew(wchar_t const* dirpath, Twins::DirectoryNotifyPtr const& nitem, LESSFN const& lessfn, bool ascending);
        void OnRemoveExisting(Twins::DirectoryNotifyPtr const& nitem);
        void OnUpdateExisting(Twins::DirectoryNotifyPtr const& nitem);
    };

    inline void List::Reserve(size_type count) { Array.reserve(count); }
    inline List::const_iterator List::Begin() const { return Array.begin(); }
    inline List::const_iterator List::End() const { return Array.end(); }
    inline Entry const& List::operator[] (int index) const { return const_cast<List*>(this)->GetByIndex(index); }
    inline Entry& List::operator[] (int index) { return GetByIndex(index); }
    inline int List::Count() const { return (int)Array.size(); }
    inline unsigned List::DirCount() const { return DirectoryCounter; }
    inline unsigned List::FileCount() const { return FileCounter; }
    inline uint64_t List::SumSize() const { return SumFileSize; }
    inline uint64_t List::MaxFileSize() const { return MaximalFileSize; }
    inline int List::MaxFilenameLen() const { return MaximalFilenameLen; }
}
