#include "stdafx.h"
#include "settings.h"
#include <dh.tracing.h>
#include <info/runtime.information.h>
#include <boost/scoped_array.hpp>
#include <iomanip>
#include <string_view>

Conf::Section& Settings()
{
    static Conf::Section root(Runtime::Info().Executable.Version.ProductName);
  //static Conf::WatchDog wd(root);
    return root;
}

namespace Conf
{
    //
    // Storage
    //
    Storage::~Storage() {}
    Storage::Storage() {}

#ifdef _WIN32
    //
    // PlatformDependedStorage
    // Get and store settings in windows registry
    //
    class PlatformDependedStorage: public Storage
    {
    public:
        PlatformDependedStorage();
        ~PlatformDependedStorage() override;

        bool Open(std::wstring_view name, int flags) override;
        void Close() override;
        std::wstring const& GetName() const override;
        bool ValueExists(std::wstring_view name) const override;
        int GetInt(std::wstring_view name) const override;
        std::wstring GetString(std::wstring_view name) const override;
        bool SetInt(std::wstring_view name, int value) const override;
        bool SetString(std::wstring_view name, std::wstring const& value) const override;
        bool LastFailed() const { return Rv != ERROR_SUCCESS; }

    private:
        friend class WatchDog;

        HKEY GetParent( int flags );
        void ResetWatchNotifier() const ;

        HKEY        BaseKey;
        std::wstring   Name;
        HANDLE ChangeNotify;
        mutable LONG     Rv;
    };

    PlatformDependedStorage::PlatformDependedStorage()
        : BaseKey(nullptr)
        , Name()
        , ChangeNotify(nullptr)
        , Rv(0)
    {
    }

    PlatformDependedStorage::~PlatformDependedStorage()
    {
        Close();
    }

    std::wstring const& PlatformDependedStorage::GetName() const
    {
        return Name;
    }

    bool PlatformDependedStorage::ValueExists(std::wstring_view name) const 
    {
        LONG rv = ::RegQueryValueExW(BaseKey, name.data(), 0, nullptr, nullptr, nullptr);
        return ERROR_SUCCESS == rv; 
    }

    HKEY PlatformDependedStorage::GetParent(int flags)
    {
        return flags & ConfLocalMachine ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;
    }

    void PlatformDependedStorage::ResetWatchNotifier() const 
    {
        if (nullptr != ChangeNotify) {
            DWORD  filter = REG_NOTIFY_CHANGE_NAME 
                          | REG_NOTIFY_CHANGE_ATTRIBUTES 
                          | REG_NOTIFY_CHANGE_LAST_SET 
                          | REG_NOTIFY_CHANGE_SECURITY
                          ;
            ::RegNotifyChangeKeyValue(BaseKey, FALSE, filter, ChangeNotify, TRUE);
        }
    }

    bool PlatformDependedStorage::Open(std::wstring_view name, int flags)
    {
        std::wstring kpath(L"SOFTWARE\\");
        kpath += name;
        HKEY parent = GetParent(flags);
        HKEY  tbase = nullptr;
        Rv = ::RegOpenKeyExW(parent, kpath.c_str(), 0, KEY_ALL_ACCESS, &tbase);
        if (ERROR_SUCCESS != Rv) {
            Rv = ::RegCreateKeyExW(parent, kpath.c_str(), 0, nullptr, 0, KEY_ALL_ACCESS, nullptr, &tbase, nullptr);
        }
        if (ERROR_SUCCESS == Rv) {
            Close();
            Name = name;
            BaseKey = tbase;
            ChangeNotify = ::CreateEventW( nullptr, FALSE, FALSE, nullptr );
            ResetWatchNotifier();
        }
        
        return Rv == ERROR_SUCCESS; 
    }

    void PlatformDependedStorage::Close()
    {
        if (nullptr != BaseKey) {
            if (nullptr != ChangeNotify) {
                ::CloseHandle(ChangeNotify);
                ChangeNotify = nullptr;
            }
            ::RegCloseKey(BaseKey);
            BaseKey = nullptr;
        }
    }

    int PlatformDependedStorage::GetInt(std::wstring_view name) const
    {
        DWORD type = REG_DWORD;
        DWORD size = sizeof(DWORD);
        DWORD data;
        Rv = ::RegQueryValueExW(BaseKey, name.data(), nullptr, &type, reinterpret_cast<LPBYTE>(&data), &size);
        return Rv == ERROR_SUCCESS ? static_cast<int>(data) : -1; 
    }

    std::wstring PlatformDependedStorage::GetString(std::wstring_view name) const
    {
        DWORD type = REG_SZ;
        DWORD size = 0;
        LSTATUS rv = 0;
        rv = ::RegQueryValueExW(BaseKey, name.data(), nullptr, &type, nullptr, &size);
        if ((ERROR_SUCCESS == rv) && (REG_SZ == type) && (size > 0)) {
            size_t                        len = size / sizeof(wchar_t);
            std::unique_ptr<wchar_t[]> buffer = std::make_unique<wchar_t[]>(len);
            rv = ::RegQueryValueExW(BaseKey, name.data(), nullptr, &type, reinterpret_cast<LPBYTE>(buffer.get()), &size);
            if ((ERROR_SUCCESS == Rv) && (size > 0)) {
                Rv = rv;
                return { buffer.get(), len - 1 };
            }
            Rv = ERROR_FILE_NOT_FOUND;
        }
        Rv = rv;
        return {};
    }

    bool PlatformDependedStorage::SetInt(std::wstring_view name, int value) const
    {
        LONG    rv = -1;
        DWORD type = REG_DWORD;
        rv = ::RegQueryValueExW(BaseKey, name.data(), 0, &type, nullptr, nullptr );
        if (ERROR_SUCCESS == rv && type != REG_DWORD) {
            return false;
        }
        Rv = ::RegSetValueExW(BaseKey, name.data(), 0, REG_DWORD, (PBYTE)&value, sizeof(DWORD));
        return ERROR_SUCCESS == Rv; 
    }

    bool PlatformDependedStorage::SetString(std::wstring_view name, std::wstring const& value) const
    {
        LONG    rv = -1;
        DWORD type = REG_SZ;
        rv = ::RegQueryValueExW(BaseKey, name.data(), 0, &type, nullptr, nullptr);
        if (ERROR_SUCCESS == rv && type != REG_SZ) {
            return false;
        }
        Rv = ::RegSetValueExW(BaseKey, name.data(), 0, REG_SZ, (PBYTE)value.c_str(), (DWORD)(value.length() * sizeof(wchar_t)));
        return ERROR_SUCCESS == Rv; 
    }
#endif // _WIN32

    static std::wstring DymmuString;

    //
    // VarBase
    // 
    VarBase::~VarBase() {}
    VarBase::VarBase() {}
    void VarBase::Assign(std::wstring_view) {}
    std::wstring const& VarBase::GetName() const { return DymmuString; }
    bool VarBase::Get(StoragePtr const&) const { return false; }
    bool VarBase::Set(StoragePtr const&) const { return false; }
    bool VarBase::IsEqual(StoragePtr const&) const { return false; }

    void VarBase::SetupStream(std::wstringstream& stream)
    {
        stream << std::setprecision(16);
    }

    Var::~Var()
    {
    }

    std::wstring_view Var::GetName() const
    {
        return nullptr == pointee_.get() ?
            std::wstring_view {} :
            std::wstring_view { pointee_->GetName().c_str(), pointee_->GetName().length() };
    }

    //
    // Section
    // 
    Section::Section(std::string const& name, unsigned flags)
        : Vars()
        , Store(new PlatformDependedStorage())
        , Flags(flags)
        , Sections()
    {
        std::wstring wname(name.length(), L' ');

#ifdef _WIN32
        ::MultiByteToWideChar(CP_ACP, 0, name.c_str(), (int)name.length(), &wname[0], (int)name.length());
#endif

        Store->Open(wname.data(), Flags);
    }

    Section::Section(std::wstring const& name, unsigned flags) 
        : Vars()
        , Store(new PlatformDependedStorage())
        , Flags(flags)
        , Sections()
    {
        Store->Open(name, Flags);
    }

    Section::Section(Section const& parent, std::wstring_view name)
        : Vars()
        , Store(new PlatformDependedStorage())
        , Flags(parent.Flags)
        , Sections()
    {
        Open(parent, name, Flags);
    }

    Section::~Section() 
    {
        if (0 != (Flags & ConfAutosave)) 
            Save();
    }

    void Section::Open(Section const& parent, std::wstring_view name, int flags)
    {
        std::wstring sub(parent.Store->GetName());
        sub += L"\\";
        sub += name;

        if (Store->Open(sub, flags)) {
            parent.Sections.push_back(this);
        }
    }

    PlatformDependedStorage* Section::GetStorage() const
    {
        return static_cast<PlatformDependedStorage*>(Store.get());
    }

    bool Section::Bind(std::wstring_view name, Var&& var) 
    {
        VarPtr newVar;
        var.pointee_.swap(newVar);
        newVar->Assign(name);
        bool rv = Store->ValueExists(name) ? newVar->Get(Store) : newVar->Set(Store);
        if (rv) {
            std::wstring_view varName = newVar->GetName();
            Vars[varName] = std::move(newVar);
        }
        return rv;
    }

    bool Section::Save() const 
    {
        size_t ok = 0;
        for (VarMap::const_iterator i = Vars.cbegin(); i != Vars.cend(); ++i) {
            if ((*i).second->Set(Store)) {
                ++ok;
            }
        }

        return Vars.size() == ok;
    }

    void Section::OnChange() const 
    {
        for (VarMap::const_iterator i=Vars.cbegin(); i!=Vars.cend(); ++i) {
            VarPtr const& vp = (*i).second;
            if (!Store->ValueExists(vp->GetName())) {
                OnParamDelete(vp);
            }
            else if (!vp->IsEqual(Store)) {
                OnParamChange(vp);
            }
        }
    }

    void Section::OnParamChange(VarPtr const& vr) const
    {
        DH::ThreadPrintf(L"SETTINGS: [%s] %s changed\n", Store->GetName().data(), vr->GetName().c_str());
        vr->Get(Store);
    }

    void Section::OnParamDelete(VarPtr const& vr) const
    {
        DH::ThreadPrintf(L"SETTINGS: [%s] %s deleted\n", Store->GetName().c_str(), vr->GetName().c_str());
    }

    //
    // WatchDog
    // 
    WatchDog::WatchDog(Section& client)
        : Client(&client)
#ifdef _WIN32
        , StopFlag(::CreateEventW(nullptr, FALSE, FALSE, nullptr))
        , WatchThread(&WatchDog::WatchProc, this)
#endif 
    {}

    WatchDog::~WatchDog() 
    { 
        Stop(); 
    }

    void WatchDog::Stop() 
    {
#ifdef _WIN32
        ::SetEvent(StopFlag);
        ::Sleep(16);
        ::CloseHandle(StopFlag);
#endif
    }

#ifdef _WIN32
    void WatchDog::WatchProc() 
    {
        DH::ScopedThreadLog l(__FUNCTIONW__);

        std::vector<HANDLE> event;
        Section::SectionVec section;

        event.reserve(Client->Sections.size() + 2);
        event.push_back(StopFlag);
        event.push_back(Client->GetStorage()->ChangeNotify);

        section.reserve(Client->Sections.size() + 1);
        section.push_back(Client);

        for (Section::SectionVec::const_iterator it = Client->Sections.cbegin(); it != Client->Sections.cend(); ++it) {
            event.push_back((*it)->GetStorage()->ChangeNotify);
            section.push_back((*it));
        }
        for (;;) {
            DWORD evt = ::WaitForMultipleObjects((DWORD)event.size(), &event[0], FALSE, INFINITE);
            if (WAIT_OBJECT_0 == evt) {
                break;
            }
            if ((evt > WAIT_OBJECT_0) && (evt < event.size())) {
                int n = evt - (WAIT_OBJECT_0 + 1);
                if ((n >= 0) && (n < (int)section.size())) {
                    section[n]->OnChange();
                    section[n]->GetStorage()->ResetWatchNotifier();
                }
            }
        }
    }
#endif
}
