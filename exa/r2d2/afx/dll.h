#ifndef aw_dll_h__
#define aw_dll_h__

#include "error.policy.h"

enum GenericDllFlags
{
    LoadDefault
,   AutoCompleteExtension   = 0x01
,   LoadAsResource          = 0x02
};

//
// ѕредставление динамически-загружаемого модул€.
// Impl - платформо-зависима€ реализаци€.
// ErrorPolicy - политика обработки ошибок.
//
template <class Impl, class ErrorPolicy = DllExceptionPolicy>
class GenericDll
{
public:
    GenericDll() throw() {}

    template <class C>
    GenericDll(C const* name, unsigned flags = AutoCompleteExtension) // throw (Exception, std::bad_alloc) 
    { 
        Load(name, flags); 
    }

    ~GenericDll() throw() { Free(); }

    template <class C>
    void Load(C const* name, unsigned flags = AutoCompleteExtension) // throw (Exception, std::bad_alloc)
    {
        if (!impl_.Load(name, flags)) 
            ErrorPolicy::LoadFailed(name, impl_.GetErrorCode());
    }

    void Free() throw() 
    {
        if (impl_.Loaded()) 
            impl_.Free(); 
    }

    template <typename Signature>
    void GetFunction(Signature& fun, char const* name) const 
    {
        fun = (Signature)impl_.GetFunction(name);
        if (!fun)
            ErrorPolicy::FunctionNotFound(name, impl_.GetErrorCode());
    }

    #define GetFunctionWithSameName(Variable) GetFunction(Variable, #Variable)

private:
    Impl impl_;
};

#endif // aw_dll_h__
