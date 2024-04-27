#pragma once

#include "directory.watch.notify.h"
#include "basic.entry.h"
#include <string>
#include <cstdint>

namespace Fl
{
    typedef std::filesystem::path Path;
    typedef std::error_code Error;

    class Entry: public Twins::Entry
    {
    public:
        Entry();
        Entry(std::wstring const& p, bool silent = false);
        Entry(wchar_t const* dirpath, Twins::DirectoryNotify const& ninfo);
        virtual ~Entry();

        Entry(Entry const& rhs);
        Entry& operator = (Entry const& rhs);
        void Swap(Entry& rhs);

        bool IsValid() const;
        void Update();
        void Update(Path const& path, uint64_t size, unsigned flags, uint64_t time, unsigned nativeattrs = 0);
        void DebugDump() const;

        std::wstring const& GetPath() const;
        std::wstring const& GetFilename() const;
        std::wstring const& GetExt() const;
        std::wstring const& GetName() const;
        uint64_t GetSize() const;
        unsigned GetFlags() const;
        unsigned GetNativeAttr() const;
        uint64_t GetTime() const;

        Path GetPathObj() const;

        bool IsDir() const;
        bool IsUpperDir() const;
        bool IsReparsePoint() const;
        bool IsHidden() const;
        bool IsFilenameEq(std::wstring const& name) const;
        bool IsRxMatch(std::wstring const& regexp, bool ignoreCase) const;
        bool IsNameStartsWith(std::wstring const& prefix, bool ignoreCase) const;

        static Entry MakeUpperDir();
        static Entry MakeStringEntry(std::wstring const& pathname);

#ifdef _WIN32
        HICON LoadShellIcon(UINT flags = SHGFI_SMALLICON) const;
        std::wstring GetShellType() const;
#endif

        std::wstring NativeAttrString() const;

    private:
        std::wstring Root;
        std::wstring Filename;
        std::wstring Extension;
        std::wstring Name;
        uint64_t Size;
        unsigned Flags;
        unsigned IntFlags;
        uint64_t Time;
    };

    typedef std::vector<Entry> EntryArray;

    inline std::wstring const& Entry::GetPath() const { return Root; }
    inline std::wstring const& Entry::GetFilename() const { return Filename; }
    inline std::wstring const& Entry::GetExt() const { return Extension; }
    inline std::wstring const& Entry::GetName() const { return Name; }
    inline uint64_t Entry::GetSize() const { return Size; }
    inline unsigned Entry::GetFlags() const { return Flags; }
    inline uint64_t Entry::GetTime() const { return Time; }
    inline unsigned Entry::GetNativeAttr() const { return IntFlags; }

    template <class T>
    static inline bool FlBitOn(T flag, T bit) { return 0 != (flag & bit); }

    inline bool Entry::IsDir() const { return FlBitOn<unsigned>(Flags, Directory); }
    inline bool Entry::IsUpperDir() const { return FlBitOn<unsigned>(Flags, UpperDirectory); }
    inline bool Entry::IsReparsePoint() const { return FlBitOn<unsigned>(Flags, Reparse); }
    inline bool Entry::IsHidden() const { return FlBitOn<unsigned>(Flags, Hidden); }
}
