#pragma once

#include "wcdafx.api.h"
#include "history.deque.h"
#include <boost/noncopyable.hpp>
#include <filesystem>
#include <string>
#include <map>

namespace Cf
{
    class DirManager: boost::noncopyable
    {
    public:
        using  Path = std::filesystem::path;
        using Ecode = std::error_code;

        WCDAFX_API ~DirManager();
        WCDAFX_API DirManager();

        WCDAFX_API void ResetToCurrent();

        Path const& FullPath() const;
        wchar_t const* FullPathname() const;
        std::wstring const& GetDrive() const;
        bool IsRootPath() const;
        HistoryDeque const& GetHistory() const;

        WCDAFX_API void SetPath(Path const& path);
        WCDAFX_API void OnChangeDrive(std::wstring const& drive);
        WCDAFX_API void RestorePrev();
        WCDAFX_API bool Go2UpperDir(std::wstring& prev);
        WCDAFX_API bool Go2RootDir(std::wstring& prev);
        WCDAFX_API void Cd(wchar_t const* path, bool isUpperDir, std::wstring& prev);

    private:
        typedef std::map<std::wstring, std::wstring> DriveMap;

        Path            Full;
        Path             Rel;
        HistoryDeque History;
        std::wstring   Drive;
        DriveMap        Dmap;

        void StoreHistory();
    };

    inline DirManager::Path const& DirManager::FullPath() const { return Full;  }
    inline wchar_t const*      DirManager::FullPathname() const { return Full.native().c_str(); }
    inline bool                  DirManager::IsRootPath() const { return Rel.empty(); }
    inline HistoryDeque const&   DirManager::GetHistory() const { return History; }
    inline std::wstring const&     DirManager::GetDrive() const { return Drive; }
}
