#include "stdafx.h"
#include "settings.h"
#include <dh.tracing.h>
#include <info/runtime.information.h>
#include <iomanip>
#include <boost/scoped_array.hpp>

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
        PlatformDependedStorage()
            : BaseKey(NULL)
            , Name()
            , ChangeNotify(NULL)
            , Rv(0)
        {}

        virtual ~PlatformDependedStorage();

        virtual bool Open(std::wstring const& name, int flags);
        virtual void Close();
        virtual std::wstring const& GetName() const;
        virtual bool ValueExists(std::wstring const& name) const;
        virtual int GetInt(std::wstring const& name) const;
        virtual std::wstring GetString(std::wstring const& name) const;
        virtual bool SetInt(std::wstring const& name, int value) const;
        virtual bool SetString(std::wstring const& name, std::wstring const& value) const;
        virtual bool LastFailed() const { return Rv != ERROR_SUCCESS; }

    private:
        friend class WatchDog;

        HKEY GetParent( int flags );
        void ResetWatchNotifier() const ;

        HKEY BaseKey;
        std::wstring Name;
        HANDLE ChangeNotify;
        mutable LONG Rv;
    };

    PlatformDependedStorage::~PlatformDependedStorage() { Close(); }

    std::wstring const& PlatformDependedStorage::GetName() const { return Name; }

    bool PlatformDependedStorage::ValueExists(std::wstring const& name) const 
    {
        LONG rv = ::RegQueryValueExW(BaseKey, name.c_str(), 0, NULL, NULL, NULL);
        return ERROR_SUCCESS == rv; 
    }

    HKEY PlatformDependedStorage::GetParent( int flags )
    {
        return flags & ConfLocalMachine ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER;
    }

    void PlatformDependedStorage::ResetWatchNotifier() const 
    {
        if (NULL != ChangeNotify)
        {
            DWORD  filter = REG_NOTIFY_CHANGE_NAME 
                          | REG_NOTIFY_CHANGE_ATTRIBUTES 
                          | REG_NOTIFY_CHANGE_LAST_SET 
                          | REG_NOTIFY_CHANGE_SECURITY
                          ;

            ::RegNotifyChangeKeyValue(BaseKey, FALSE, filter, ChangeNotify, TRUE);
        }
    }

    bool PlatformDependedStorage::Open(std::wstring const& name, int flags )
    {
        std::wstring kpath(L"SOFTWARE\\");
        kpath += name;


        HKEY parent = GetParent(flags);
        HKEY tbase = NULL;

        Rv = ::RegOpenKeyExW(parent, kpath.c_str(), 0, KEY_ALL_ACCESS, &tbase);
        if (ERROR_SUCCESS != Rv)
            Rv = ::RegCreateKeyExW(parent, kpath.c_str(), 0, NULL, 0, KEY_ALL_ACCESS, NULL, &tbase, NULL);

        if (ERROR_SUCCESS == Rv) 
        {
            Close();

            Name = name;
            BaseKey = tbase;
            ChangeNotify = ::CreateEventW( NULL, FALSE, FALSE, NULL );

            ResetWatchNotifier();
        }
        
        return Rv == ERROR_SUCCESS; 
    }

    void PlatformDependedStorage::Close()
    {
        if (NULL != BaseKey)
        {
            if (NULL != ChangeNotify)
            {
                ::CloseHandle(ChangeNotify);
                ChangeNotify = NULL;
            }
                
            ::RegCloseKey(BaseKey);
            BaseKey = NULL;
        }
    }

    int PlatformDependedStorage::GetInt(std::wstring const& name) const
    {
        DWORD type = REG_DWORD;
        DWORD size = sizeof(DWORD);
        DWORD data;
        Rv = ::RegQueryValueExW(BaseKey, name.c_str(), NULL, &type, (LPBYTE)&data, &size);
        return Rv == ERROR_SUCCESS ? static_cast<int>(data) : -1; 
    }

    std::wstring PlatformDependedStorage::GetString(std::wstring const& name) const
    {
        DWORD type = REG_SZ;
        DWORD size = 0;

        Rv = ::RegQueryValueExW(BaseKey, name.c_str(), NULL, &type, NULL, &size);
        if (ERROR_SUCCESS == Rv)
        {
            boost::scoped_array<wchar_t> buffer(new wchar_t[size + 1]);
            ZeroMemory(buffer.get(), size + 1);

            Rv = ::RegQueryValueExW(BaseKey, name.c_str(), NULL, &type, (LPBYTE)buffer.get(), &size);
            return std::wstring(Rv == ERROR_SUCCESS ? buffer.get() : L"");
        }
        
        return std::wstring();
    }

    bool PlatformDependedStorage::SetInt(std::wstring const& name, int value) const
    {
        LONG rv = -1;
        DWORD type = REG_DWORD;

        rv = ::RegQueryValueExW(BaseKey, name.c_str(), 0, &type, NULL, NULL );
        if (ERROR_SUCCESS == rv && type != REG_DWORD) 
            return false;

        Rv = ::RegSetValueExW(BaseKey, name.c_str(), 0, REG_DWORD, (PBYTE)&value, sizeof(DWORD));
        return ERROR_SUCCESS == Rv; 
    }

    bool PlatformDependedStorage::SetString(std::wstring const& name, std::wstring const& value) const
    {
        LONG rv = -1;
        DWORD type = REG_SZ;

        rv = ::RegQueryValueExW(BaseKey, name.c_str(), 0, &type, NULL, NULL);
        if (ERROR_SUCCESS == rv && type != REG_SZ) 
            return false;

        Rv = ::RegSetValueExW(BaseKey, name.c_str(), 0, REG_SZ
            , (PBYTE)value.c_str(), (DWORD)(value.length() * sizeof(wchar_t)));

        return ERROR_SUCCESS == Rv; 
    }
#endif // _WIN32

    static std::wstring DymmuString;

    //
    // VarBase
    // 
    VarBase::~VarBase() {}
    VarBase::VarBase() {}
    void VarBase::Assign(std::wstring const&) {}
    std::wstring const& VarBase::GetName() const { return DymmuString; }
    bool VarBase::Get(StoragePtr const&) const { return false; }
    bool VarBase::Set(StoragePtr const&) const { return false; }
    bool VarBase::Eq(StoragePtr const&) const { return false; }

    void VarBase::SetupStream(std::wstringstream& stream)
    {
        stream << std::setprecision(16)
               ;
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

        Store->Open(wname.c_str(), Flags);
    }

    Section::Section(std::wstring const& name, unsigned flags) 
        : Vars()
        , Store(new PlatformDependedStorage())
        , Flags(flags)
        , Sections()
    {
        Store->Open(name, Flags);
    }

    Section::Section(Section const& parent, std::wstring const& name)
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

    void Section::Open(Section const& parent, std::wstring const& name, int flags)
    {
        std::wstring sub(parent.Store->GetName());
        sub += L"\\";
        sub += name;

        if (Store->Open(sub, flags)) 
            parent.Sections.push_back(this);
    }

    PlatformDependedStorage* Section::GetStorage() const
    {
        return (PlatformDependedStorage*)Store.get();
    }

    bool Section::Bind(std::wstring const& name, Var var) 
    {
        VarPtr x = var.Ptr;
       
        x->Assign(name);
        bool rv = Store->ValueExists(name) ? x->Get(Store) : x->Set(Store);
        if (rv) 
            Vars[name] = x;

        return rv;
    }

    bool Section::Save() const 
    {
        size_t ok = 0;

        for (VarMap::const_iterator i = Vars.begin(); i != Vars.end(); ++i)
        {
            if ((*i).second->Set(Store))
                ++ok;
        }

        return Vars.size() == ok;
    }

    void Section::OnChange() const 
    {
        for (VarMap::const_iterator i=Vars.begin(); i!=Vars.end(); ++i) 
        {
            VarPtr const& vp = (*i).second;

            if (!Store->ValueExists(vp->GetName()))
                OnParamDelete(vp);

            else if (!vp->Eq(Store)) 
                OnParamChange(vp);
        }
    }

    void Section::OnParamChange(VarPtr const& vr) const
    {
        Dh::ThreadPrintf(L"SETTINGS: [%s] %s changed\n", Store->GetName().c_str(), vr->GetName().c_str());
        vr->Get(Store);
    }

    void Section::OnParamDelete(VarPtr const& vr) const
    {
        Dh::ThreadPrintf(L"SETTINGS: [%s] %s deleted\n", Store->GetName().c_str(), vr->GetName().c_str());
    }

    //
    // WatchDog
    // 
    WatchDog::WatchDog(Section& client)
        : Client(&client)
#ifdef _WIN32
        , StopFlag(::CreateEventW(NULL, FALSE, FALSE, NULL))
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
        Dh::ScopedThreadLog l(__FUNCTIONW__);

        std::vector<HANDLE> event;
        Section::SectionVec section;

        event.reserve(Client->Sections.size() + 2);
        event.push_back(StopFlag);
        event.push_back(Client->GetStorage()->ChangeNotify);

        section.reserve(Client->Sections.size() + 1);
        section.push_back(Client);

        for (Section::SectionVec::const_iterator it = Client->Sections.begin(); it != Client->Sections.end(); ++it)
        {
            event.push_back((*it)->GetStorage()->ChangeNotify);
            section.push_back((*it));
        }

        for (;;) 
        {
            DWORD evt = ::WaitForMultipleObjects((DWORD)event.size(), &event[0], FALSE, INFINITE);

            if (WAIT_OBJECT_0 == evt)
                break;

            else if ((evt > WAIT_OBJECT_0) && (evt < event.size()))
            {
                int n = evt - (WAIT_OBJECT_0 + 1);

                if ((n >= 0) && (n < (int)section.size()))
                {
                    section[n]->OnChange();
                    section[n]->GetStorage()->ResetWatchNotifier();
                }
            }
        }
    }
#endif
}

Conf::Section& Settings()
{
    static Conf::Section root(Runtime::Info().Executable.Version.ProductName);
  //static Conf::WatchDog wd(root);
    return root;
}
