#include "stdafx.h"
#if 0
#include "fs.enum.h"
#include "fs.links.h"
#include "record.info.h"
#include <dh.tracing.h>
#include <string.utils.human.size.h>
#include <string.utils.error.code.h>
#include <exception>
#include <atlfile.h>

namespace Twins
{
    namespace Fs
    {
        bool IsReparsePoint(WIN32_FIND_DATA const& data)
        {
            return 0 != (FILE_ATTRIBUTE_REPARSE_POINT & data.dwFileAttributes);
        }

        bool IsSymLink(WIN32_FIND_DATA const& data)
        {
            return IsReparsePoint(data) && (0 != (IO_REPARSE_TAG_SYMLINK & data.dwReserved0));
        }

        void LinkWithSeparator(CString& dest, PCWSTR source)
        {
            int len = dest.GetLength();
            if ((len > 1) && (_T('\\') != dest[len-1]))
                dest += _T("\\");

            dest += source;
        }

        CString LinkFilename(PCTSTR path, PCTSTR dir, WIN32_FIND_DATA const& data)
        {
            CString rv = path;
            LinkWithSeparator(rv, dir);
            LinkWithSeparator(rv, data.cFileName);
            return rv;
        }

        CString LinkFilename(PCTSTR drive, PCTSTR path, PCTSTR dir)
        {
            CString rv = drive;
            LinkWithSeparator(rv, path);
            LinkWithSeparator(rv, dir);
            return rv;
        }

        CString LinkFilename(PCTSTR drive, PCTSTR path, PCTSTR dir, PCTSTR filename)
        {
            CString rv = drive;
            LinkWithSeparator(rv, path);
            LinkWithSeparator(rv, dir);
            LinkWithSeparator(rv, filename);
            return rv;
        }

        CString LinkFilename(PCTSTR path, PCTSTR dir)
        {
            CString rv = path;
            LinkWithSeparator(rv, dir);
            return rv;
        }

        static inline void AddFlag(CString& dest, PCTSTR flagStr, DWORD flag, DWORD mask)
        {
            if (0 != (mask & flag))
                dest += flagStr;
        }

        CString AttrToString(DWORD attr)
        {
            CString rv;

            AddFlag(rv, _T("R"), attr, FILE_ATTRIBUTE_READONLY);
            AddFlag(rv, _T("H"), attr, FILE_ATTRIBUTE_HIDDEN);
            AddFlag(rv, _T("S"), attr, FILE_ATTRIBUTE_SYSTEM);
            AddFlag(rv, _T("D"), attr, FILE_ATTRIBUTE_DIRECTORY);
            AddFlag(rv, _T("A"), attr, FILE_ATTRIBUTE_ARCHIVE);
            AddFlag(rv, _T("X"), attr, FILE_ATTRIBUTE_DEVICE);
            AddFlag(rv, _T("N"), attr, FILE_ATTRIBUTE_NORMAL);
            AddFlag(rv, _T("T"), attr, FILE_ATTRIBUTE_TEMPORARY);
            AddFlag(rv, _T("S"), attr, FILE_ATTRIBUTE_SPARSE_FILE);
            AddFlag(rv, _T("P"), attr, FILE_ATTRIBUTE_REPARSE_POINT);
            AddFlag(rv, _T("C"), attr, FILE_ATTRIBUTE_COMPRESSED);
            AddFlag(rv, _T("O"), attr, FILE_ATTRIBUTE_OFFLINE);
            AddFlag(rv, _T("I"), attr, FILE_ATTRIBUTE_NOT_CONTENT_INDEXED);
            AddFlag(rv, _T("E"), attr, FILE_ATTRIBUTE_ENCRYPTED);
            AddFlag(rv, _T("V"), attr, FILE_ATTRIBUTE_VIRTUAL);

            return rv;
        }

        void Split(CString const& filename, bool isDir, CString& name, CString& ext)
        {
            if (isDir)
                name = filename;

            else
            {
                int n = filename.ReverseFind(_T('.'));
                if ((-1 == n) || !n)
                    name = filename;

                else
                {
                    int el = filename.GetLength() - n - 1;
                    if (el > 0)
                        ext = filename.Right(el);

                    name = filename.Left(n);
                }
            }
        }

        void Split(EntryInfo const& ent, CString& name, CString& ext)
        {
            Split(ent.name, ent.isDir, name, ext);
        }

        EntryInfo::EntryInfo(PCTSTR path, PCTSTR dir, WIN32_FIND_DATA const& data)
            : path(path)
            , dir(dir)
            , name(data.cFileName)
            , shortName(data.cAlternateFileName)
            , pathName(LinkFilename(path, dir, data))
            , size(FileSize(data))
            , attr(data.dwFileAttributes)
            , isHidden(IsHidden(data))
            , isDir(IsDir(data))
            , isUpDir(IsUpDir(data))
            , isLink(IsSymLink(data))
        {
        }

        void EntryInfo::Dump() const
        {
            Dh::TPrintf(_T("EntryNfo: %08x | %16s | %s %s %s\n")
                , attr
                , Str::HumanSize(size)
                , pathName
                , (isLink ? _T("->") : _T(""))
                , (isLink ? QueryLinkTarget(pathName) : _T(""))
                );
        }

        bool IsHidden(WIN32_FIND_DATA const& data)
        {
            return 0 != (data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN);
        }

        bool IsDir(DWORD attrs)
        {
            return 0 != (attrs & FILE_ATTRIBUTE_DIRECTORY);
        }

        bool IsDir(WIN32_FIND_DATA const& data)
        {
            return IsDir(data.dwFileAttributes);
        }

        unsigned __int64 FileSize(WIN32_FIND_DATA const& data)
        {
            return (unsigned __int64)data.nFileSizeLow | (((unsigned __int64)data.nFileSizeHigh) << 32);
        }

        bool IsNotDir(WIN32_FIND_DATA const& data)
        {
            return !IsDir(data);
        }

        bool IsCurrentDir(WIN32_FIND_DATA const& data)
        {
            return IsDir(data) && ((_T('.') == data.cFileName[0]) && (_T('\0') == data.cFileName[1]));
        }

        bool IsUpDir(WIN32_FIND_DATA const& data)
        {
            return IsDir(data) && ((_T('.') == data.cFileName[0]) && (_T('.') == data.cFileName[1]) && (_T('\0') == data.cFileName[2]));
        }

        Enumerator::~Enumerator() {}

        void Enumerator::operator() (PCTSTR rawPath, PCTSTR rawDir, WIN32_FIND_DATA const& data)
        {
            EntryInfo info(rawPath, rawDir, data);
            info.Dump();
        }

        bool Enumerator::Stop() const
        {
            return false;
        }

        static void PutUpperDir(PCTSTR rawPath, PCTSTR rawDir, Enumerator& enumerator)
        {
            if (::_tcslen(rawDir) > 0)
            {
                WIN32_FIND_DATA data = {0};
                _tcsncpy_s(data.cFileName, _T(".."), 3);
                _tcsncpy_s(data.cAlternateFileName, _T(".."), 3);
                data.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
                enumerator(rawPath, rawDir, data);
            }
        }

        static HRESULT OnEnumError(PCTSTR rawPath, PCTSTR rawDir, PCTSTR rawMask, Enumerator& enumerator, HRESULT hr)
        {
            Dh::TPrintf(_T("EnmError: `%s%s\\%s` %s %d (0x%x)\n"), rawPath, rawDir, rawMask, Str::ErrorCode<TCHAR>::SystemMessage(hr), hr, hr);

            CString pathName = LinkFilename(rawPath, rawDir);
            DWORD attrs = ::GetFileAttributes(pathName);
            if (((DWORD)-1 != attrs) && (0 != ((FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT) & attrs)))
            {
                CString targetPath = QueryLinkTarget(pathName);
                if (targetPath.IsEmpty())
                    PutUpperDir(rawPath, rawDir, enumerator);

                else
                    hr = Enumerate(targetPath, _T(""), rawMask, enumerator);
            }
            else
                PutUpperDir(rawPath, rawDir, enumerator);

            return hr;
        }

        struct AutoFindHandle
        {
            AutoFindHandle(HANDLE h)
                : h_(h) 
            {}

            ~AutoFindHandle() 
            {
                Close(); 
            }

            bool Valid() const { return INVALID_HANDLE_VALUE != h_; }
            HANDLE Get() const { return h_; }

            void Close()
            { 
                if (Valid()) 
                { 
                    ::FindClose(h_); 
                    h_ = INVALID_HANDLE_VALUE; 
                } 
            }

            void Reset(HANDLE h)
            {
                Close(); 
                h_ = h; 
            }

        private:
            HANDLE h_;
        };

        HRESULT Enumerate(PCTSTR rawPath, PCTSTR rawDir, PCTSTR rawMask, Enumerator& enumerator)
        {
            HRESULT rv = S_OK;

            try
            {
                CString searchMask = LinkFilename(rawPath, rawDir);
                LinkWithSeparator(searchMask, rawMask);

                WIN32_FIND_DATA data = {0};
                AutoFindHandle fh = ::FindFirstFile(searchMask, &data);
                if (!fh.Valid())
                    return OnEnumError(rawPath, rawDir, rawMask, enumerator, ::GetLastError());

                if (enumerator.Stop())
                    return ERROR_CANCELLED;

                if (!IsCurrentDir(data))
                    enumerator(rawPath, rawDir, data);

                while (::FindNextFile(fh.Get(), &data))
                {
                    if (enumerator.Stop())
                    {
                        rv = ERROR_CANCELLED;
                        break;
                    }

                    enumerator(rawPath, rawDir, data);
                }
            }
            catch (std::exception const& ex)
            {
                Dh::TPrintf("EFsError: %s\n", __FUNCTION__, ex.what());
                rv = ERROR_BAD_CURRENT_DIRECTORY;
            }

            return rv;
        }

        HRESULT NewDirectory(PCWSTR drive, PCWSTR path, PCWSTR dir)
        {
            CString temp = LinkFilename(drive, path, dir);
            return ::SHCreateDirectoryExW(NULL, temp, NULL);
        }

        CString GetDir(CString const& pathName)
        {
            int n = pathName.ReverseFind(_T('\\'));
            return pathName.Left(n);
        }

        HRESULT Rename(PCWSTR pathname, PCWSTR newName)
        {
            CString newPathname = GetDir(pathname);
            LinkWithSeparator(newPathname, newName);

            if (!::MoveFileW(pathname, newPathname))
            {
                HRESULT hr = ::GetLastError();
                return S_OK == hr ? ERROR_INVALID_ACCESS : hr;
            }

            return S_OK;
        }

        bool IsDir(PCWSTR path, CString const& filename)
        {
            CStringW pathname = LinkFilename(path, filename);
            DWORD attrs = ::GetFileAttributesW(pathname);

            if (INVALID_FILE_ATTRIBUTES != attrs)
                return IsDir(attrs);

            return false;
        }

        void UpdateFileTime(RecordInfo& info, FILETIME const& creation, FILETIME const& lastAccess, FILETIME const& lastWrite)
        {
            info.creationTime = CTime(CFileTime(creation));
            info.lastAccessTime = CTime(CFileTime(lastAccess));
            info.lastWriteTime = CTime(CFileTime(lastWrite));

            info.date = info.GetDisplayTime().Format(_T("%x %X")); 
        }

        void UpdateFileTime(RecordInfo& info, WIN32_FIND_DATA const& data)
        {
            UpdateFileTime(info, data.ftCreationTime, data.ftLastAccessTime, data.ftLastWriteTime);
        }

        void UpdateFileSize(RecordInfo& info, bool isDir, SizeType size)
        {
            info.fileSize = size;
            info.size = isDir ? _T("") : Str::HumanSize(size);
        }

        bool UpdateEntry(RecordInfo& info)
        {
            bool rv = false;

            DWORD attrs = ::GetFileAttributesW(info.pathName);
            if (INVALID_FILE_ATTRIBUTES != attrs)
            {
                info.attr = attrs;

                CAtlFile file;
                HRESULT hr = file.Create(info.pathName, FILE_READ_ATTRIBUTES //| (info.IsDir() ? FILE_LIST_DIRECTORY : 0)
                    , FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
                    , OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS);

                if (SUCCEEDED(hr))
                {
                    FILETIME creation = {};
                    FILETIME lastAccess = {}; 
                    FILETIME lastWrite = {};

                    if (::GetFileTime(file, &creation, &lastAccess, &lastWrite))
                        UpdateFileTime(info, creation, lastAccess, lastWrite);

                    LARGE_INTEGER tempSize = {};
                    if (::GetFileSizeEx(file, &tempSize))
                        UpdateFileSize(info, IsDir(attrs), (SizeType)tempSize.QuadPart);

                    rv = true;
                }
            }

            return rv;
        }

        bool GetFileAttributesAndSize(CString const& pathname, DWORD& sourceAttrs, SizeType& sourceSize)
        {
            bool rv = false;

            DWORD attrs = ::GetFileAttributes(pathname);
            if (INVALID_FILE_ATTRIBUTES != attrs)
            {
                CAtlFile file;
                HRESULT hr = file.Create(pathname, FILE_READ_ATTRIBUTES
                    , FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
                    , OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS);

                if (SUCCEEDED(hr))
                {
                    LARGE_INTEGER tempSize = {};
                    if (::GetFileSizeEx(file, &tempSize))
                    {
                        rv = true;
                        sourceAttrs = attrs;
                        sourceSize = (SizeType)tempSize.QuadPart;
                    }
                }
            }

            return rv;
        }

#if 0
        void SplitByPathName(CString const& source, CString& path, CString& name)
        {
            int ln = source.GetLength();
            int ls = source.ReverseFind(_T("\\"));
            if (-1 != ls)
            {
                path = source.Left(ls);
                name = source.Right(ln - ls);
            }
        }
#endif

        CString GetFilename(CString const& source)
        {
            int ln = source.GetLength();
            int ls = source.ReverseFind(_T('\\'));
            return source.Right(ln - (ls + 1));
        }
    }
}
#endif // 0
