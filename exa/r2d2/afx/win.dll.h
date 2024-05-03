#ifndef aw_win_dll_h__
#define aw_win_dll_h__

#include <string>
#include "dll.h"
#include "use.windows.h"

class DllImpl
{
public:
    DllImpl() throw() : mod_(NULL) {}
    ~DllImpl() throw() {}

    template <class C>
    bool Load(C const* name, unsigned flags) // throw(std::bad_alloc) 
    {
        std::basic_string<C> dllname(name);
        unsigned arg = 0;

#if 0   // FIXME: enable this
        if (LoadAsResource & flags) arg |= LOAD_LIBRARY_AS_IMAGE_RESOURCE;
#endif

        if (AutoCompleteExtension & flags)
            dllname += SelectVersion<C>::Extension();

        mod_ = SelectVersion<C>::Load(dllname.c_str(), arg);

        return NULL != mod_;
    }

    bool Loaded() const throw() { return NULL != mod_; }
    void Free() throw() { FreeLibrary(mod_); }

    int GetErrorCode() const throw() { return GetLastError(); }

    void *GetFunction(char const* name) const 
    {
        return (void*)GetProcAddress(mod_, name);
    }

private:
    template <class U> struct SelectVersion {};

    template <> struct SelectVersion<char> 
    {
        static HMODULE Load(char const* name, unsigned flags) { return LoadLibraryExA(name, NULL, flags); }
        static char const* Extension() { return ".DLL"; }
    };

    template <> struct SelectVersion<wchar_t> 
    {
        static HMODULE Load(wchar_t const* name, unsigned flags) { return LoadLibraryExW(name, NULL, flags); }
        static wchar_t const* Extension() { return L".DLL"; }
    };

    HMODULE mod_;
};

typedef GenericDll<DllImpl> DynamicLibrary;

#endif // aw_win_dll_h__
