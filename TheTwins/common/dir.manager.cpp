#include "stdafx.h"
#include "dir.manager.h"

namespace Cf
{
    DirManager::DirManager()
        : Rel()
        , Full()
        , History()
        , Drive()
        , Dmap()
    {}

    DirManager::~DirManager()
    {}

    void DirManager::SetPath(Path const& path) 
    {
        Rel = path.relative_path(); 

        if (Drive.empty())
            Drive = path.wstring().substr(0, 3);

        Full = Drive + Rel.native();
        StoreHistory(); 
    }

    void DirManager::StoreHistory()
    {
        std::wstring item = Full.native();
        History.Push(item);
    }

    void DirManager::RestorePrev()
    {
        SetPath(History.Pop().c_str());
    }

    void DirManager::OnChangeDrive(std::wstring const& drive)
    {
        if (!Drive.empty() && !Rel.empty())
            Dmap[Drive] = Rel.native();

        Drive = drive;
        DriveMap::iterator di = Dmap.find(drive);
        
        SetPath(di != Dmap.end() ? di->second.c_str() : L"");
        StoreHistory();
    }

    bool DirManager::Go2UpperDir(std::wstring& prev)
    {
        if (!Rel.empty())
        {
            prev = Rel.filename().native();

            SetPath(Rel.parent_path());
            return true;
        }

        return false;
    }

    bool DirManager::Go2RootDir(std::wstring& prev)
    {
        if (IsRootPath())
            return false;

        prev = Rel.begin()->filename().native();

        SetPath(L"\\");
        return true;
    }

    void DirManager::Cd(wchar_t const* path, bool isUpperDir, std::wstring& prev)
    {
        Path temp;
    
        if (isUpperDir && !Rel.empty())
        {
            prev = Rel.filename().native();
            temp = Drive + Rel.parent_path().native();
        }
        else
            temp = path;
    
        SetPath(temp);
    }
}
