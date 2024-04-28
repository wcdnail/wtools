#pragma once 

#if 0

#include <windows.h>
#include <string>

namespace CF // Common framework
{
    class ModuleHelper
    {
    public:
        typedef std::basic_string<TCHAR> MyString;

        static ModuleHelper& Instance();

        void Reset(HMODULE mod);

        HMODULE GetHandle() const { return mod_; }
        MyString const& GetPath() const { return path_; }
        MyString const& GetName() const { return name_; }
        
    private:
        ModuleHelper(); /* throw (std::bad_alloc) */
        ~ModuleHelper();

        static void GetPathName(HMODULE mod, MyString& path, MyString& name); /* throw (std::bad_alloc) */

        HMODULE mod_;
        MyString path_;
        MyString name_;
    };

    inline ModuleHelper& ModuleHelper::Instance()
    {
        static ModuleHelper inst;
        return inst;
    }

    inline ModuleHelper::ModuleHelper() 
        : mod_(NULL)
        , path_()
        , name_()
    {
        Reset(::GetModuleHandle(NULL));
    }

    inline void ModuleHelper::Reset(HMODULE mod)
    {
        mod_ = mod;
        GetPathName(mod_, path_, name_);
    }

    inline ModuleHelper::~ModuleHelper() 
    {
    }

    inline void ModuleHelper::GetPathName(HMODULE mod, MyString& path, MyString& name)
    {
        TCHAR buffer[8192] = {0};
        DWORD l = ::GetModuleFileName(mod, buffer, _countof(buffer));
        if (l > 0)
        {
            MyString tempPath;
            MyString tempName;

            MyString temp = buffer;
            MyString::size_type n = temp.rfind(_T('\\'));
            if (MyString::npos != n)
            {
                ++n;
                tempPath = temp.substr(0, n);
                tempName = temp.substr(n, l-n);
            }
            else
            {
                tempName = temp;
            }

            path.swap(tempPath);
            name.swap(tempName);
        }
    }
}

#endif
