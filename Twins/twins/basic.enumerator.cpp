#include "stdafx.h"
#include "basic.enumerator.h"
#include "ntfs.links.h"
#include "basic.entry.h"
#include <filesystem>

namespace Twins
{
    Enumerator::Enumerator()
        : Stop(false)
    {}

    Enumerator::~Enumerator()
    {}

    Enumerator::LoadArgs::LoadArgs(std::wstring const& root, std::wstring const& mask, bool nohidden, bool noupper, bool recursive)
        : Root(root)
        , Mask(mask)
        , SkipHidden(nohidden)
        , SkipUpper(noupper)
        , Recursive(recursive)
        , Deep(0)
    {}

    void Enumerator::OnEntry(Item const& params) {}
    void Enumerator::Cancel() { Stop = true; }
    bool Enumerator::Canceled() const { return Stop; }
    void Enumerator::Reset() { Stop = false; }

#ifdef _WIN32
    static bool IsDir(DWORD attrs) { return 0 != (attrs & FILE_ATTRIBUTE_DIRECTORY); }
    static bool IsDir(WIN32_FIND_DATAW const& data) { return IsDir(data.dwFileAttributes); }
    static bool IsCurrentDir(WIN32_FIND_DATAW const& data) { return IsDir(data) && ((L'.' == data.cFileName[0]) && (L'\0' == data.cFileName[1])); }
    static bool IsUpperDir(WIN32_FIND_DATAW const& data) { return IsDir(data) && ((L'.' == data.cFileName[0]) && (L'.' == data.cFileName[1]) && (L'\0' == data.cFileName[2])); }
    static bool IsReparsePoint(WIN32_FIND_DATAW const& data) { return 0 != (FILE_ATTRIBUTE_REPARSE_POINT & data.dwFileAttributes); }
    static bool IsSymLink(WIN32_FIND_DATAW const& data) { return IsReparsePoint(data) && (0 != (IO_REPARSE_TAG_SYMLINK & data.dwReserved0)); }
    static bool IsHidden(WIN32_FIND_DATAW const& data) { return 0 != (data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN); }

    unsigned Win32FindDataToFlags(WIN32_FIND_DATAW const& fdata)
    {
        unsigned flags;

        flags = (Entry::Normal)
              | (IsDir(fdata)           ? Entry::Directory        : 0)
              | (IsCurrentDir(fdata)    ? Entry::CurrentDirectory : 0)
              | (IsUpperDir(fdata)      ? Entry::UpperDirectory   : 0)
              | (IsSymLink(fdata)       ? Entry::Symlink          : 0)
              | (IsReparsePoint(fdata)  ? Entry::Reparse          : 0)
              | (IsHidden(fdata)        ? Entry::Hidden           : 0)
              ;

        return flags;
    }

    Enumerator::Item::Item(WIN32_FIND_DATAW const& data, LoadArgs const& params)
        : Args(params)
        , Name(data.cFileName)
        , Size(((uint64_t)data.nFileSizeLow | (((uint64_t)data.nFileSizeHigh) << 32)))
// ##TODO: Configure display time"))
        , Time(*((uint64_t*)&data.ftLastWriteTime)) // ftCreationTime 
        , Flags(Win32FindDataToFlags(data))
        , Int(data.dwFileAttributes)
    {}

    ATL::CTime Enumerator::Item::GetAtlTime() const { return CTime(*((PFILETIME)&Time)); }
    bool Enumerator::Item::IsDir() const { return 0 != (Entry::Directory & Flags); }
    bool Enumerator::Item::IsUpperDir() const { return 0 != (Entry::UpperDirectory & Flags); }
    bool Enumerator::Item::IsCurrentDir() const { return 0 != (Entry::CurrentDirectory & Flags); }
    bool Enumerator::Item::IsHidden() const { return 0 != (Entry::Hidden & Flags); }

    static const WIN32_FIND_DATAW CurrentDirFindData = { FILE_ATTRIBUTE_DIRECTORY, {0}, {0}, {0}, 0, 0, 0, 0, L".", L"." };
    static const WIN32_FIND_DATAW UpperDirFindData = { FILE_ATTRIBUTE_DIRECTORY, {0}, {0}, {0}, 0, 0, 0, 0, L"..", L".." };

    Enumerator::Ec Enumerator::Load(LoadArgs const& args)
    {
        Ec ec;
        std::filesystem::path searchpath = args.Root.c_str();
        searchpath /= args.Mask.c_str();

        WIN32_FIND_DATAW data = {0};
        HANDLE handle = ::FindFirstFileExW(searchpath.c_str()
                                         , FindExInfoBasic              // cAlternateFileName is always NULL
                                         , &data
                                         , FindExSearchNameMatch
                                         , NULL
                                         , FIND_FIRST_EX_LARGE_FETCH);  // Uses a larger buffer for directory queries, Win7 and higher

        if (INVALID_HANDLE_VALUE == handle)
        {
            searchpath = args.Root;

            DWORD attrs = ::GetFileAttributesW(searchpath.c_str());
            if ((INVALID_FILE_ATTRIBUTES != attrs) && (0 != ((FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT) & attrs)))
            {
                searchpath = Ntfs::QueryLinkTarget(searchpath.wstring(), ec);
                if (ec)
                {
                    if (!args.SkipUpper)
                        OnEntry(Item(UpperDirFindData, args));

                    return ec;
                }

                searchpath /= args.Mask.c_str();
                handle = ::FindFirstFileW(searchpath.c_str(), &data);
                if (INVALID_HANDLE_VALUE == handle)
                {
                    if (!args.SkipUpper)
                        OnEntry(Item(UpperDirFindData, args));

                    return Ec(::GetLastError(), std::system_category());
                }
            }
        }

        ec.clear();
        std::shared_ptr<void> autoFind(handle, FindClose);

        if (!IsCurrentDir(data))
            OnEntry(Item(data, args));

        while (::FindNextFileW(handle, &data) && !ec)
        {
            if (Canceled())
            {
                ec = Ec(ERROR_CANCELLED, std::system_category());
                break;
            }

            if (IsUpperDir(data) && args.SkipUpper)
                continue ;

            OnEntry(Item(data, args));

            if (IsDir(data) && args.Recursive)
            {
                std::filesystem::path rpath = args.Root.c_str();
                rpath /= data.cFileName;

                LoadArgs rargs(rpath.c_str(), args.Mask, args.SkipHidden, args.SkipUpper, args.Recursive);
                rargs.Deep = args.Deep + 1;

                Load(rargs);
            }
        }

        return ec;
    }
#endif // _WIN32
}
