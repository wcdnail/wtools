#include "stdafx.h"
#include "file.entry.h"
#include <dh.tracing.h>
#include <boost/regex_fwd.hpp>
#include <boost/regex.hpp>
#include <boost/filesystem/operations.hpp>
#ifdef _WIN32
#include <shlwapi.h>
#endif 

namespace Twins 
{ 
    unsigned Win32FindDataToFlags(WIN32_FIND_DATAW const&); 
}

namespace Fl
{
    static const std::wstring InvalidPath = L"\0";
    static const uint64_t InvalidSize = (uint64_t)-1;
    static const unsigned InvalidFlags = (unsigned)-1;

    static void GetNativeProps(Path const& path, uint64_t& size, unsigned& flags, uint64_t& time, unsigned& nativeattrs, Error& ec)
    {
#ifdef _WIN32
        WIN32_FIND_DATAW fdata = {};
        HANDLE hfound = ::FindFirstFileW(path.c_str(), &fdata);
        if (INVALID_HANDLE_VALUE != hfound)
        {
            nativeattrs = fdata.dwFileAttributes;
            time = *((uint64_t*)&fdata.ftLastAccessTime);
            size = (uint64_t)fdata.nFileSizeLow | (((uint64_t)fdata.nFileSizeHigh) << 32);
            flags = Twins::Win32FindDataToFlags(fdata);
            ec.clear();

            ::FindClose(hfound);
        }
        else
            ec.assign((int)::GetLastError(), std::system_category());
#endif // _WIN32
    }

    static Error InitFromPath(Path const& path, Entry& target)
    {
        uint64_t size = 0;
        unsigned flags = 0;
        uint64_t time = 0;
        unsigned attrs;

        Error ec;
        GetNativeProps(path.native(), size, flags, time, attrs, ec);
        target.Update(path, size, flags, time, attrs);
        return ec;
    }

    static Error InitFromNotify(wchar_t const* dirpath, Twins::DirectoryNotify const& ninfo, Entry& target)
    {
        Path path = dirpath;
        path /= ninfo.Filename.GetString();

        return InitFromPath(path, target);
    }

    Entry::~Entry()
    {
    }

    Entry::Entry()
        : Root(InvalidPath)
        , Filename()
        , Extension()
        , Name()
        , Size(InvalidSize)
        , Flags(InvalidFlags)
        , IntFlags(0)
        , Time(0)
    {
    }

    Entry::Entry(std::wstring const& p, bool silent /*= false*/)
        : Root(InvalidPath)
        , Filename()
        , Extension()
        , Name()
        , Size(InvalidSize)
        , Flags(InvalidFlags)
        , IntFlags(0)
        , Time(0)
    {
        Entry temp;
        Error ec = InitFromPath(p.c_str(), temp);

        if (!ec)
            temp.Swap(*this);

        else if (!silent)
            Dh::ThreadPrintf("DirEntry: `%S` %d %s\n", p.c_str(), ec.value(), ec.message().c_str());
    }

    Entry::Entry(wchar_t const* dirpath, Twins::DirectoryNotify const& ninfo)
        : Root(InvalidPath)
        , Filename()
        , Extension()
        , Name()
        , Size(InvalidSize)
        , Flags(InvalidFlags)
        , IntFlags(0)
        , Time(0)
    {
        Entry temp;
        Error ec = InitFromNotify(dirpath, ninfo, temp);

        if (!ec)
            temp.Swap(*this);
        else
            Dh::ThreadPrintf("DirEntry: `%S %S` %d %s\n", dirpath, ninfo.Filename, ec.value(), ec.message().c_str());
    }

    Entry::Entry(Entry const& rhs)
        : Root(rhs.Root)
        , Filename(rhs.Filename)
        , Extension(rhs.Extension)
        , Name(rhs.Name)
        , Size(rhs.Size)
        , Flags(rhs.Flags)
        , IntFlags(rhs.IntFlags)
        , Time(rhs.Time)
    {
    }

    Entry& Entry::operator = (Entry const& rhs)
    {
        Entry temp(rhs);
        temp.Swap(*this);
        return *this;
    }

    void Entry::Swap(Entry& rhs)
    {
        Root.swap(rhs.Root);
        Filename.swap(rhs.Filename);
        Extension.swap(rhs.Extension);
        Name.swap(rhs.Name);
        std::swap(Size, rhs.Size);
        std::swap(Flags, rhs.Flags);
        std::swap(IntFlags, rhs.IntFlags);
        std::swap(Time, rhs.Time);
    }

    bool Entry::IsValid() const
    {
        return !(Root == InvalidPath)
            && !(Size == InvalidSize)
            && !(Flags == InvalidFlags)
            ;
    }

    void Entry::Update(Path const& path, uint64_t size, unsigned flags, uint64_t time, unsigned nativeattrs)
    {
        Root = path.native();
        Size = size;
        Flags = flags;
        IntFlags = nativeattrs;
        Time = time;

        Path filename = path.filename();
        Filename = filename.native();
        Extension = filename.extension().native();
        Name = filename.replace_extension().native();
    }

    void Entry::DebugDump() const
    {
#ifdef _DEBUG
        Dh::ThreadPrintf(L"DirEntry: | %08x | %20I64u | %s\n", Flags, Size, Root.c_str());
#endif
    }

    Entry Entry::MakeUpperDir()
    {
        Entry rv;
        rv.Root = L"..";
        rv.Filename = rv.Name = rv.Root;
        rv.Size = 0;
        rv.Flags = Directory | UpperDirectory;
        return rv;
    }

    Entry Entry::MakeStringEntry(std::wstring const& pathname)
    {
        Entry rv;
        rv.Update(pathname.c_str(), 0, 0, 0, 0);
        return rv;
    }

    void Entry::Update()
    {
        Entry temp(Root.c_str());
        if (temp.IsValid())
            Swap(temp);
    }

    bool Entry::IsFilenameEq(std::wstring const& name) const
    {
        return Filename == name;
    }

    bool Entry::IsRxMatch(std::wstring const& regexp, bool ignoreCase) const
    {
        boost::wregex rex(regexp, (ignoreCase ? boost::wregex::icase : boost::wregex::normal));
        boost::wcmatch m;
        if (boost::regex_match(Filename.c_str(), m, rex))
            return true;

        return false;
    }

    bool Entry::IsNameStartsWith(std::wstring const& prefix, bool ignoreCase) const
    {
        PWSTR start = (PWSTR)Filename.c_str();

        PWSTR subst = ignoreCase ?
#ifdef _WIN32
                    ::StrStrIW(start, prefix.c_str()) :
#endif 
                    ::wcsstr(start, prefix.c_str());

        return subst && start == subst;
    }

    Path Entry::GetPathObj() const 
    {
        return Path(Root.c_str()); 
    }

#ifdef _WIN32
    HICON Entry::LoadShellIcon(UINT flags /*= SHGFI_SMALLICON | SHGFI_LINKOVERLAY*/) const
    {
        SHFILEINFO info = {};
        DWORD_PTR rv = ::SHGetFileInfoW(Root.c_str(), IntFlags, &info, sizeof(info), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | flags);
#pragma message(_TODO("Handle `NULL == info.hIcon`"))
        HICON icon = info.hIcon;
        return icon;
    }

    WidecharString Entry::GetShellType() const
    {
        SHFILEINFO info = {};

        DWORD_PTR rv = ::SHGetFileInfoW(Root.c_str(), IntFlags, &info, sizeof(info)
            , SHGFI_USEFILEATTRIBUTES 
            | SHGFI_TYPENAME
            );

        return info.szTypeName;
    }
#endif

    static inline void AddFlag(std::wstring& dest, wchar_t const* string, unsigned flag, unsigned mask)
    {
        dest += (0 != (mask & flag)) ? string : L"-";
    }

    std::wstring Entry::NativeAttrString() const
    {
        std::wstring rv;

#ifdef _WIN32
        AddFlag(rv, L"r", IntFlags, FILE_ATTRIBUTE_READONLY);
        AddFlag(rv, L"h", IntFlags, FILE_ATTRIBUTE_HIDDEN);
        AddFlag(rv, L"s", IntFlags, FILE_ATTRIBUTE_SYSTEM);
        AddFlag(rv, L"d", IntFlags, FILE_ATTRIBUTE_DIRECTORY);
        AddFlag(rv, L"a", IntFlags, FILE_ATTRIBUTE_ARCHIVE);
        AddFlag(rv, L"x", IntFlags, FILE_ATTRIBUTE_DEVICE);
        AddFlag(rv, L"n", IntFlags, FILE_ATTRIBUTE_NORMAL);
        AddFlag(rv, L"t", IntFlags, FILE_ATTRIBUTE_TEMPORARY);
        AddFlag(rv, L"s", IntFlags, FILE_ATTRIBUTE_SPARSE_FILE);
        AddFlag(rv, L"p", IntFlags, FILE_ATTRIBUTE_REPARSE_POINT);
        AddFlag(rv, L"c", IntFlags, FILE_ATTRIBUTE_COMPRESSED);
        AddFlag(rv, L"o", IntFlags, FILE_ATTRIBUTE_OFFLINE);
        AddFlag(rv, L"i", IntFlags, FILE_ATTRIBUTE_NOT_CONTENT_INDEXED);
        AddFlag(rv, L"e", IntFlags, FILE_ATTRIBUTE_ENCRYPTED);
        AddFlag(rv, L"v", IntFlags, FILE_ATTRIBUTE_VIRTUAL);
#endif

        return rv;
    }
}
