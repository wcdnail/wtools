//
// 30.07.2013 MNi: Refactored from 05.08.2009
//

#pragma once

#include "wcdafx.api.h"
#include "rectz.h"
#include "settings.details.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <sstream>

namespace Conf
{
    enum MiscFlags
    { 
      // use this flags for all MS Windows apps
      // ignored under *nix
      ConfLocalMachine = 0x0001,
      ConfCurrentUser = 0x0002,

      // next values used by Section class for autosave 
      ConfAutosave = 0x0004,
    };

    class Section;
    class WatchDog;
    class PlatformDependedStorage;

    //
    // Configuration storage interface.
    // Under MS Windows - it's a registry.
    // Under *nix it's mau be a usual text file.
    //
    class Storage
    {
        DELETE_COPY_MOVE_OF(Storage);
    public:
        WCDAFX_API virtual ~Storage();
        virtual bool Open(std::wstring_view name, int flags = ConfCurrentUser) = 0;
        virtual void Close() = 0;
        virtual std::wstring const& GetName() const = 0;
        virtual bool ValueExists(std::wstring_view name) const = 0;
        virtual int GetInt(std::wstring_view name) const = 0;
        virtual std::wstring GetString(std::wstring_view name ) const = 0;
        virtual bool SetInt(std::wstring_view name, int value) const = 0;
        virtual bool SetString(std::wstring_view name, std::wstring const& value) const = 0;
        virtual bool LastFailed() const = 0;
    protected:
        WCDAFX_API Storage();
    };

    typedef std::unique_ptr<Storage> StoragePtr;
    
    //
    // VarBase
    //
    struct VarBase
    {
        DELETE_COPY_MOVE_OF(VarBase);

        WCDAFX_API virtual ~VarBase();
        virtual void Assign(std::wstring_view) = 0;
        virtual std::wstring const& GetName() const = 0;
        virtual bool Get(StoragePtr const& conf) const = 0;
        virtual bool Set(StoragePtr const& conf) const = 0;
        virtual bool IsEqual(StoragePtr const& conf) const = 0;

    protected:
        WCDAFX_API VarBase();
        WCDAFX_API static void SetupStream(std::wstringstream& stream);
    };

    using VarPtr = std::unique_ptr<VarBase>;

    //
    // VarT
    // 
    template <typename T>
    class VarT: public VarBase
    {
    public:
        VarT(T& v): Name(), Pointee(&v) {}
        ~VarT() override {}
        void Assign(std::wstring_view name) override { Name = name; }
        std::wstring const& GetName() const override { return Name; }
        bool Get(StoragePtr const&) const override;
        bool Set(StoragePtr const&) const override;
        bool IsEqual(StoragePtr const&) const override;
    private:
        std::wstring Name;
        T*        Pointee;
        VarT(T& v, std::wstring_view name): Name(name), Pointee(&v) {}
    };

    template <> 
    inline bool VarT<int>::Get(StoragePtr const& store) const 
    { 
        *Pointee = store->GetInt(Name); 
        return !store->LastFailed(); 
    }

    template <> 
    inline bool VarT<int>::IsEqual(StoragePtr const& store) const 
    {
        int y = store->GetInt(Name); 
        return *Pointee == y; 
    }

    template <> 
    inline bool VarT<int>::Set(StoragePtr const& store) const 
    {
        return store->SetInt(Name, *Pointee); 
    }

    template <> 
    inline bool VarT<std::wstring>::Get(StoragePtr const& store) const 
    {
        *Pointee = store->GetString(Name); 
        return !store->LastFailed(); 
    }

    template <> 
    inline bool VarT<std::wstring>::IsEqual(StoragePtr const& store) const 
    {
        return *Pointee == store->GetString(Name); 
    }

    template <> 
    inline bool VarT<std::wstring>::Set(StoragePtr const& store) const 
    {
        return store->SetString(Name, *Pointee); 
    }

    template <typename T> 
    inline bool VarT<T>::Get(StoragePtr const& store) const 
    {
        std::wstring const temp{store->GetString(Name)};
        if (!store->LastFailed()) {
            std::wstringstream input(temp);
            VarBase::SetupStream(input);
            input >> *Pointee;
            return true;
        }
        return false; 
    }

    template <typename T>
    inline bool VarT<T>::IsEqual(StoragePtr const& store) const 
    { 
        T temp;
        VarT<T> const tv{temp, {Name.c_str(), Name.length()}};
        if (!tv.Get(store)) {
            return true;
        }
        return temp == *Pointee;
    }

    template <typename T> 
    inline bool VarT<T>::Set(StoragePtr const& store) const 
    {
        std::wstringstream output;
        VarBase::SetupStream(output);
        output << *Pointee;
        return store->SetString(Name, output.str()); 
    }

    //
    // Var
    // 
    struct Var
    {
        VarPtr pointee_;

        WCDAFX_API ~Var();
        WCDAFX_API std::wstring_view GetName() const;

        template <typename T> Var(T& v);
    };

    template <typename T>
    inline Var::Var(T& v)
        : pointee_(std::make_unique<VarT<T>>(v))
    {
    }

    //
    // Связывает обычные переменные
    // с их конфигурационным представлением 
    //
    class Section
    {
        DELETE_COPY_MOVE_OF(Section);

    public:
        WCDAFX_API Section(std::string const& name, unsigned flags = ConfCurrentUser | ConfAutosave);
        WCDAFX_API Section(std::wstring const& name, unsigned flags = ConfCurrentUser | ConfAutosave);
        
        // 
        // для корректной работы Watcher
        // под-ветки необходимо создавать с помощью этих конструкторов 
        //
        WCDAFX_API Section(Section const& parent, std::wstring_view name);
        WCDAFX_API ~Section();

        WCDAFX_API bool Bind(std::wstring_view name, Var&& var);
        WCDAFX_API bool Save() const;

    private:
        friend class WatchDog;

        using     VarMap = std::unordered_map<std::wstring_view, VarPtr>;
        using SectionVec = std::vector<Section*>;

        virtual void OnChange() const;
        virtual void OnParamChange(VarPtr const& vr) const;
        virtual void OnParamDelete(VarPtr const& vr) const;

        void Open(Section const& parent, std::wstring_view name, int flags);
        PlatformDependedStorage* GetStorage() const;

        VarMap                 Vars;
        StoragePtr            Store;
        unsigned              Flags;
        mutable SectionVec Sections;
    };

    class WatchDog
    {
        DELETE_COPY_MOVE_OF(WatchDog);

    public:
        WCDAFX_API ~WatchDog();
        WCDAFX_API WatchDog(Section& client);

    private:
        Section*         Client;
#ifdef _WIN32
        HANDLE         StopFlag;
        std::thread WatchThread;

        void WatchProc();
#endif
        void Stop();
    };
}

extern WCDAFX_API Conf::Section& Settings();

#define FromSettings(Sect, Variable) \
    Sect.Bind(L#Variable, Conf::Var(Variable))
