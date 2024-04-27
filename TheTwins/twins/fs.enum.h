#pragma once

#if 0
#include <atlstr.h>

namespace Twins
{
    struct RecordInfo;

    namespace Fs
    {
        typedef unsigned __int64 SizeType;

        HRESULT NewDirectory(PCWSTR drive, PCWSTR path, PCWSTR dir);
        HRESULT Rename(PCWSTR pathname, PCWSTR newName);
        bool IsDir(PCWSTR path, CString const& filename);
        void UpdateFileTime(RecordInfo& info, WIN32_FIND_DATA const& data);
        void UpdateFileSize(RecordInfo& info, bool isDir, SizeType size);
        bool UpdateEntry(RecordInfo& info);
        //void SplitByPathName(CString const& source, CString& path, CString& name);
        CString GetFilename(CString const& source);
        bool GetFileAttributesAndSize(CString const& pathname, DWORD& sourceAttrs, SizeType& sourceSize);

        struct EntryInfo
        {
            CString path;
            CString dir;
            CString name;
            CString shortName;
            CString pathName;
            SizeType size;
            DWORD attr;
            bool isHidden;
            bool isDir;
            bool isUpDir;
            bool isLink;

            EntryInfo(PCTSTR path, PCTSTR dir, WIN32_FIND_DATA const& data);
            void Dump() const;
        };

        CString AttrToString(DWORD attr);
        bool IsReparsePoint(WIN32_FIND_DATA const& data);
        bool IsSymLink(WIN32_FIND_DATA const& data);
        void LinkWithSeparator(CString& dest, PCWSTR source);
        CString LinkFilename(PCTSTR path, PCTSTR dir, WIN32_FIND_DATA const& data);
        CString LinkFilename(PCTSTR path, PCTSTR dir);
        CString LinkFilename(PCTSTR drive, PCTSTR path, PCTSTR dir);
        CString LinkFilename(PCTSTR drive, PCTSTR path, PCTSTR dir, PCTSTR filename);
        CString ModeToString(unsigned short mode);
        bool IsHidden(WIN32_FIND_DATA const& data);
        bool IsDir(WIN32_FIND_DATA const& data);
        unsigned __int64 FileSize(WIN32_FIND_DATA const& data);
        bool IsNotDir(WIN32_FIND_DATA const& data);
        bool IsCurrentDir(WIN32_FIND_DATA const& data);
        bool IsUpDir(WIN32_FIND_DATA const& data);
        void Split(CString const& filename, bool isDir, CString& name, CString& ext);
        void Split(EntryInfo const& ent, CString& name, CString& ext);
        CString GetDir(CString const& pathName);

        struct Enumerator
        {
            virtual ~Enumerator();
            virtual void operator() (PCTSTR path, PCTSTR dir, WIN32_FIND_DATA const& data);
            virtual bool Stop() const;
        };

        HRESULT Enumerate(PCTSTR rawPath, PCTSTR rawDir, PCTSTR rawMask, Enumerator& enumerator);
    }
}
#endif
