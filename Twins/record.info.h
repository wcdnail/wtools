#pragma once

#if 0
#include <atlstr.h>
#include <atltime.h>

namespace Twins
{
    struct DirectoryNotify;

    struct RecordInfo
    {
        typedef bool (*LessFn)(RecordInfo const&, RecordInfo const&, bool);
        typedef unsigned __int64 SizeType;

        enum InternalType
        {
            NotInitialized = -1,
            UpperDirectory = 0,
            Directory,
            File,
        };

        RecordInfo();
        RecordInfo(CString const& path, DirectoryNotify const& ninfo);

        RecordInfo(RecordInfo const& rhs);
        RecordInfo& operator = (RecordInfo const& rhs);
        void Swap(RecordInfo& rhs);

        CString name;
        CString type;
        CString size;
        CString date;
        //CString mode;
        DWORD attr;
        CTime creationTime;
        CTime lastAccessTime;
        CTime lastWriteTime;
        CString pathName;
        unsigned __int64 fileSize;
        int intType;

        void ResetIntType();
        CTime const& GetDisplayTime() const;
        bool IsValid() const;
        bool IsDir() const;
        bool IsUpDir() const;
        bool IsReparsePoint() const;
        bool IsHidden() const;

        void UpdateName(CString const& text);
        CString GetName() const;

        bool Match(CString const& regexp, bool ignoreCase) const;

        static bool LessByName(RecordInfo const& L, RecordInfo const& R, bool ascending);
        static bool LessByType(RecordInfo const& L, RecordInfo const& R, bool ascending);
        static bool LessBySize(RecordInfo const& L, RecordInfo const& R, bool ascending);
        static bool LessByDate(RecordInfo const& L, RecordInfo const& R, bool ascending);

        static bool SameName(RecordInfo const& L, CString const& name);
        static bool SameNameAndType(RecordInfo const& L, CString const& name, CString const& type);
        static bool NameStartsWith(RecordInfo const& L, CString const& prefix, bool ignoreCase);

    private:
        void RebuildPathname();
    };
}
#endif // 0
