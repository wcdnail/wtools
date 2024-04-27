//
// 30.07.2013 MNi: Refactored from 05.08.2009
//

#pragma once

#include "wcdafx.api.h"
#include "rectz.h"
#include "settings.details.h"
#include <string>
#include <memory>
#include <sstream>
#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>

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
    // Under *nix it's a usual text file.
    //
    class Storage: boost::noncopyable
    {
    public:
        WCDAFX_API virtual ~Storage() = 0;

        virtual bool Open(std::wstring const& name, int flags = ConfCurrentUser) = 0;
        virtual void Close() = 0;
        virtual std::wstring const& GetName() const = 0;
        virtual bool ValueExists(std::wstring const& name) const = 0;
        virtual int GetInt(std::wstring const& name) const = 0;
        virtual std::wstring GetString(std::wstring const& name ) const = 0;
        virtual bool SetInt(std::wstring const& name, int value) const = 0;
        virtual bool SetString(std::wstring const& name, std::wstring const& value) const = 0;
        virtual bool LastFailed() const = 0;

    protected:
        WCDAFX_API Storage();
    };

    typedef std::unique_ptr<Storage> StoragePtr;
    
    //
    // VarBase
    //
    class VarBase: boost::noncopyable
    {
    public:
        WCDAFX_API virtual ~VarBase() = 0;

        virtual void Assign(std::wstring const&) = 0;
        virtual std::wstring const& GetName() const = 0;

        virtual bool Get(StoragePtr const& conf) const = 0;
        virtual bool Set(StoragePtr const& conf) const = 0;
        virtual bool Eq(StoragePtr const& conf) const = 0;

    protected:
        WCDAFX_API VarBase();

        WCDAFX_API static void SetupStream(std::wstringstream& stream);
    };

    typedef boost::shared_ptr<VarBase> VarPtr;

    //
    // VarT
    // 
    template <typename T>
    class VarT: public VarBase
    {
    public:
        VarT(T& v)
            : Name()
            , Pointee(&v) 
        {}

        virtual ~VarT() {}

        virtual void Assign(std::wstring const& name) { Name = name; }
        virtual std::wstring const& GetName() const { return Name; }

        virtual bool Get(StoragePtr const&) const;
        virtual bool Set(StoragePtr const&) const;
        virtual bool Eq(StoragePtr const&) const;

    private:
        std::wstring Name;
        T *Pointee;

        VarT(T& v, std::wstring const& name)
            : Name(name)
            , Pointee(&v) 
        {}
    };

    template <> 
    inline bool VarT<int>::Get(StoragePtr const& store) const 
    { 
        *Pointee = store->GetInt(Name); 
        return !store->LastFailed(); 
    }

    template <> 
    inline bool VarT<int>::Eq(StoragePtr const& store) const 
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
    inline bool VarT<std::wstring>::Eq(StoragePtr const& store) const 
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
        std::wstring temp = store->GetString(Name);
        if (!store->LastFailed())
        {
            std::wstringstream input(temp);
            VarBase::SetupStream(input);
            input >> *Pointee;
            return true;
        }

        return false; 
    }

    template <typename T> 
    inline bool VarT<T>::Eq(StoragePtr const& store) const 
    { 
        T temp;
        VarT<T> tv(temp, Name);

        if (!tv.Get(store))
            return true;

        return *Pointee == temp;
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
    class Var
    {
    public:
        template <typename T>
        Var(T& v)
            : Ptr(new VarT<T>(v))
        {}

    private:
        friend class Section;

        VarPtr Ptr;
    };

    //
    // Связывает обычные переменные
    // с их конфигурационным представлением 
    //
    class Section: boost::noncopyable
    {
    public:
        WCDAFX_API Section(std::string const& name, unsigned flags = ConfCurrentUser | ConfAutosave);
        WCDAFX_API Section(std::wstring const& name, unsigned flags = ConfCurrentUser | ConfAutosave);
        
        // 
        // для корректной работы Watcher
        // под-ветки необходимо создавать с помощью этих конструкторов 
        //
        WCDAFX_API Section(const Section & parent, std::wstring const& name);
        WCDAFX_API ~Section();

        WCDAFX_API bool Bind(std::wstring const& name, Var var);
        WCDAFX_API bool Save() const;

    private:
        friend class WatchDog;

        typedef boost::unordered_map<std::wstring, VarPtr> VarMap;
        typedef std::vector<Section*> SectionVec;

        virtual void OnChange() const;
        virtual void OnParamChange(VarPtr const& vr) const;
        virtual void OnParamDelete(VarPtr const& vr) const;

        void Open(const Section & parent, std::wstring const& name, int flags);
        PlatformDependedStorage* GetStorage() const;

        VarMap Vars;
        StoragePtr Store;
        unsigned Flags;
        mutable SectionVec Sections;
    };

    class WatchDog: boost::noncopyable
    {
    public:
        WCDAFX_API WatchDog(Section& client);
        WCDAFX_API ~WatchDog();

    private:
        Section *Client;
#ifdef _WIN32
        HANDLE StopFlag;
        std::thread WatchThread;

        void WatchProc();
#endif

        void Stop();
    };
}

extern WCDAFX_API Conf::Section& Settings();

#define FromSettings(Sect, Variable) \
    Sect.Bind(L#Variable, Conf::Var(Variable))
