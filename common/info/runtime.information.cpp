#include "stdafx.h"
#include "runtime.information.h"
#include "dh.tracing.h"
#include "string.utils.error.code.h"
#include "string.utils.env.vars.h"
#include <boost/config.hpp>
#include <boost/version.hpp>
#include <filesystem>
#include <cstdint>
#include <cstring>

#ifdef _MSC_VER
#  pragma warning(disable: 4191) // 4191: 'reinterpret_cast': unsafe conversion from 'FARPROC' to 'GPI'
                                 // 4191:         Making a function call using the resulting pointer may cause your program to fail
#  pragma warning(disable: 4996) // 4996: 'GetVersionExW': was declared deprecated
#  pragma warning(disable: 4710) // 4710: 'HRESULT StringCchPrintfW(STRSAFE_LPWSTR,size_t,STRSAFE_LPCWSTR,...)': function not inlined
#  pragma warning(disable: 4514) // 4514: 'StringCchCopyA': unreferenced inline function has been removed
#endif

#ifdef _WIN32
#  include <atlstr.h>
#endif

#define _MKWIDE_(S)  L#S
#define _MK_WIDE(S) _MKWIDE_(S)

namespace Runtime
{
    wchar_t const* InfoStore::Compiler      = _MK_WIDE(BOOST_COMPILER);
    wchar_t const* InfoStore::CLib          = _MK_WIDE(BOOST_STDLIB);
    wchar_t const* InfoStore::BoostVersion  = _MK_WIDE(BOOST_LIB_VERSION);
    wchar_t const* InfoStore::TargetOs      = _MK_WIDE(BOOST_PLATFORM)
#ifdef _WIN32
#  ifdef _WIN64
        L" x64"
#  endif
        L" " _MK_WIDE(BOOST_STRINGIZE(_WIN32_WINNT))
#endif
        ;

#ifdef _WIN32
    namespace
    {
        struct AutoLibraryHandle
        {
            AutoLibraryHandle(HMODULE lib)
                : lib_(lib)
            {}

            ~AutoLibraryHandle()
            {
                if (NULL != lib_)
                {
                    ::FreeLibrary(lib_);
                    lib_ = NULL;
                }
            }

            operator bool () const
            {
                return NULL != lib_;
            }

            template <class T>
            bool GetAddress(PCSTR name, T& functionPtr)
            {
                FARPROC temp = ::GetProcAddress(lib_, name);
                functionPtr = (T)temp;
                return NULL != temp;
            }

        private:
            HMODULE lib_;
        };

        struct AutoGlobalPtr
        {
            AutoGlobalPtr(SIZE_T cb, UINT flags = GMEM_MOVEABLE)
                : heap_(::GlobalAlloc(flags, cb))
            {
            }

            ~AutoGlobalPtr() 
            {
                ::GlobalUnlock(heap_);
                ::GlobalFree(heap_);
            }

            template <class T>
            T Lock() const
            {
                return (T)::GlobalLock(heap_);
            }

        private:
            HGLOBAL heap_;
        };
    }

    static bool RequestWin32VersionInfo(PCWSTR pathName, InfoStore::ExecutableInfo::VersionInfo& Version)
    {
        struct LangInfo
        {
            WORD lang;
            WORD   cp;
        };
        struct VarDef
        {
            PCWSTR                   varName;
            InfoStore::String* destStringPtr;
        };

        typedef DWORD (APIENTRY *GetFileVersionInfoSizeWPtr)(PCWSTR, LPDWORD);
        typedef BOOL (APIENTRY *GetFileVersionInfoWPtr)(PCWSTR, DWORD, DWORD, LPVOID);
        typedef BOOL (APIENTRY *VerQueryValueWPtr)(LPCVOID, PCWSTR, LPVOID*, PUINT);

        GetFileVersionInfoSizeWPtr pGetFileVersionInfoSizeW = nullptr;
        GetFileVersionInfoWPtr         pGetFileVersionInfoW = nullptr;
        VerQueryValueWPtr                   pVerQueryValueW = nullptr;
        PSTR                                         lpData = nullptr;
        LangInfo*                                  langInfo = nullptr;
        DWORD                                         dwLen = 0;
        UINT                                          uSize = 0;
        DWORD                                    lpdwHandle = 0;


        AutoLibraryHandle version(::LoadLibrary(_T("version")));
        if (!(version
           && version.GetAddress("GetFileVersionInfoSizeW", pGetFileVersionInfoSizeW)
           && version.GetAddress("GetFileVersionInfoW", pGetFileVersionInfoW)
           && version.GetAddress("VerQueryValueW", pVerQueryValueW)
           )) {
            return false;
        }

        dwLen = pGetFileVersionInfoSizeW(pathName, &lpdwHandle);
        if (dwLen < 1) {
            return false;
        }
        AutoGlobalPtr heap(dwLen);
        lpData = heap.Lock<PSTR>();
        if (!pGetFileVersionInfoW(pathName, lpdwHandle, dwLen, lpData)) {
            return false;
        }
        if (!pVerQueryValueW(lpData, L"\\VarFileInfo\\Translation", reinterpret_cast<PVOID*>(&langInfo), &uSize)) {
            return false;
        }

        VarDef varDef[] =
        {
            { L"ProductName"         , &Version.ProductName          },
            { L"ProductVersion"      , &Version.ProductVersion       },
            { L"FileDescription"     , &Version.FileDescription      },
            { L"FileVersion"         , &Version.FileVersion          },
            { L"Comments"            , &Version.Comments             },
            { L"InternalName"        , &Version.InternalName         },
            { L"CompanyName"         , &Version.CompanyName          },
            { L"LegalCopyright"      , &Version.LegalCopyright       },
            { L"LegalTrademarks"     , &Version.LegalTrademarks      },
            { L"PrivateBuild"        , &Version.PrivateBuild         },
            { L"OriginalFilename"    , &Version.OriginalFilename     },
            { L"SpecialBuild"        , &Version.SpecialBuild         },
        };

        for (auto& it: varDef) {
            wchar_t section[64] = { 0 };
            int rv = swprintf_s(section, L"\\StringFileInfo\\%04x%04x\\%s", langInfo->lang, langInfo->cp, it.varName);
            if (rv > 0) {
                PWSTR value = nullptr;
                if (pVerQueryValueW(lpData, section, reinterpret_cast<PVOID*>(&value), &uSize)) {
                    *(it.destStringPtr) = value;
                }
#ifdef _DEBUG_XTRA
                else {
                    wchar_t traceMsg[128] = { 0 };
                    swprintf_s(traceMsg, L"\t>>>> VerQueryValue fails with '%s' '%s'\n", it.varName, section);
                    OutputDebugStringW(traceMsg);
                }
#endif
            }
        }
        return true;
    }
#endif

    static void RequestExeInfo(InfoStore::ExecutableInfo& exeInfo)
    {
#ifdef _WIN32
        STARTUPINFOW startupInfo = {0};
        startupInfo.cb = sizeof(startupInfo);
        GetStartupInfoW(&startupInfo);

        InfoStore::String temp = startupInfo.lpTitle;
        InfoStore::String::size_type n = temp.rfind(L'\\');
        n = (InfoStore::String::npos == n ? 0 : n + 1);
        InfoStore::String tempDir = temp.substr(0, n);
        InfoStore::String tempName = temp.substr(n, temp.length() - n);

        exeInfo.Directory = tempDir;
        exeInfo.Filename = tempName;

        if (!RequestWin32VersionInfo(startupInfo.lpTitle, exeInfo.Version)) {
            exeInfo.Version.ProductName = std::filesystem::path(exeInfo.Filename).filename().replace_extension().wstring();
        }
#endif
    }

    static void RequestHostInfo(InfoStore::HostInfo& hostInfo)
    {
#ifdef _WIN32

#ifdef _DEBUG_EXTRA_INFO
        {
            TCHAR cmDescription[8][32] = 
            {
                TEXT("NetBIOS"), 
                TEXT("DNS hostname"), 
                TEXT("DNS domain"), 
                TEXT("DNS fully-qualified"), 
                TEXT("Physical NetBIOS"), 
                TEXT("Physical DNS hostname"), 
                TEXT("Physical DNS domain"), 
                TEXT("Physical DNS fully-qualified")
            };

            for (int cnf = 0; cnf < ComputerNameMax; cnf++)
            {
                TCHAR buffer[256] = {0};
                DWORD size = _countof(buffer);
                BOOL rv = ::GetComputerNameEx((COMPUTER_NAME_FORMAT)cnf, buffer, &size);

                DH::TPrintf(_T("CompName: `%s` (%s)\n")
                    , (!rv ? (PCTSTR)Str::ErrorCode<>::SystemMessage(GetLastError()) : buffer)
                    , cmDescription[cnf]
                    );
            }
        }
#endif 
        wchar_t tempName[256] = {0};
        DWORD tempNameLen = _countof(tempName);
        if (::GetComputerNameExW(ComputerNamePhysicalDnsFullyQualified, tempName, &tempNameLen)) {
            hostInfo.Name = tempName;
        }
#endif
    }

    static void RequestUserInfo(InfoStore::UserInfo& userInfo)
    {
#ifdef _WIN32
        wchar_t tempName[256] = {0};
        DWORD tempNameLen = _countof(tempName);
        if (::GetUserNameW(tempName, &tempNameLen)) {
            userInfo.Name = tempName;
        }
#endif
    }

    InfoStore::SystemInfo::VersionInfo::VersionInfo()
        : Major(0)
        , Minor(0)
        , DisplayName()
    {
    }

    InfoStore::SystemInfo::VersionInfo::~VersionInfo()
    {}

#ifdef _WIN32
#   define ENV_SYSTEMROOT    L"%SYSTEMROOT%"
#   define ENV_HOME          L"%USERPROFILE%"
#else
#   define ENV_SYSTEMROOT    L"$ROOT"
#   define ENV_HOME          L"$HOME"
#endif

    InfoStore::SystemInfo::SystemInfo()
        : CpuType(0)
        , CpuNum(1)
        , PageSize(4096)
        , AllocationGranularity(0)
        , RootDirectory(Env::Expand(ENV_SYSTEMROOT))
        , HomeDirectory(Env::Expand(ENV_HOME))
        , Version()
    {}

    InfoStore::SystemInfo::~SystemInfo()
    {}

    Endianness InfoStore::SystemInfo::GetEndian()
    {
        union 
        {
            uint8_t  c[4];
            uint32_t i;
        } u;

        u.i = 0x01020304;

        if (0x04 == u.c[0])
            return LittleEndian;

        else if (0x01 == u.c[0])
            return BigEndian;
        
        return UnknownEndian;
    }

    wchar_t const* InfoStore::SystemInfo::GetEndianString()
    {
        static wchar_t const* const es[] = { L"Unknown", L"LittleEndian", L"BigEndian" };
        return es[(int)(GetEndian()) + 1];
    }

    InfoStore::ExecutableInfo::VersionInfo::VersionInfo()
        : ProductName()
        , ProductVersion()
        , FileDescription()
        , FileVersion()
        , Comments()
        , InternalName()
        , CompanyName()
        , LegalCopyright()
        , LegalTrademarks()
        , PrivateBuild()
        , OriginalFilename()
        , SpecialBuild()
    {
    }

    InfoStore::ExecutableInfo::VersionInfo::~VersionInfo()
    {}

    InfoStore::HostInfo::HostInfo()
        : Name(L"localhost")
    {}

    InfoStore::HostInfo::~HostInfo()
    {}

    InfoStore::UserInfo::UserInfo()
        : Name()
    {}

    InfoStore::UserInfo::~UserInfo()
    {}

    InfoStore::ExecutableInfo::ExecutableInfo()
        : Directory()
        , Filename()
        , Version()
    {}

    InfoStore::ExecutableInfo::~ExecutableInfo()
    {}

    InfoStore::InfoStore()
        : System()
        , Executable()
        , Host()
        , User()
    {
        System.Initialize();
        RequestExeInfo(Executable);
        RequestHostInfo(Host);
        RequestUserInfo(User);
    }

    InfoStore::~InfoStore()
    {
    }

    void InfoStore::SimpleReport(OStream& stream) const
    {
        stream 
         << L"Compiler                            : " << Compiler << L"\n"
            L"C library                           : " << CLib << L"\n"
            L"Boost version                       : " << BoostVersion << L"\n"
            L"Target os                           : " << TargetOs << L"\n"
            L"System endian                       : " << System.GetEndianString() << L"\n"
            L"System cpu info                     : " << System.CpuType << " x" << System.CpuNum << L"\n"
            L"System page size                    : " << System.PageSize << L"\n"
            L"System allocation granularity       : " << System.AllocationGranularity << L"\n"
            L"System name                         : " << System.Version.DisplayName.c_str() << L"\n"
            L"System version                      : " << System.Version.Major << "." << System.Version.Minor << L"\n"
            L"System root                         : " << System.RootDirectory.c_str() << L"\n"
            L"Home directory                      : " << System.HomeDirectory.c_str() << L"\n"
            L"Executable directory                : " << Executable.Directory.c_str() << L"\n"
            L"Executable name                     : " << Executable.Filename.c_str() << L"\n"
            L"Executable product name             : " << Executable.Version.ProductName.c_str() << L"\n"
            L"Executable version                  : " << Executable.Version.ProductVersion.c_str() << L"\n"
            L"Executable special build            : " << Executable.Version.SpecialBuild.c_str() << L"\n"
            L"Host name                           : " << Host.Name.c_str() << L"\n"
            L"User name                           : " << User.Name.c_str() << L"\n"
         ;
    }

    InfoStore::String InfoStore::SimpleReport() const
    {
        StringStream temp;
        SimpleReport(temp);
        return temp.str();
    }

    InfoStore const& Info()
    {
        static InfoStore instance;
        return instance;
    }

    static InfoStore::String GetOsDisplayString(
#ifdef _WIN32
                                                 OSVERSIONINFOEXW& osvi
                                               , SYSTEM_INFO& si
#endif // _WIN32
                                               )
    {
        InfoStore::StringStream temp;

#ifdef _WIN32
        temp << L"Windows ";

        if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId && osvi.dwMajorVersion > 4)
        {
            if (osvi.dwMajorVersion >= 6)
            {
                switch (osvi.dwMinorVersion)
                {
                case 0: temp << (VER_NT_WORKSTATION == osvi.wProductType ? L"Vista " : L"Server 2008 "); break;
                case 1: temp << (VER_NT_WORKSTATION == osvi.wProductType ? L"7 "     : L"Server 2008 R2 ");  break;
                case 2: temp << (VER_NT_WORKSTATION == osvi.wProductType ? L"8 "     : L"Server 2012 "); break;
                }

                DWORD ptype = static_cast<DWORD>(-1);
                typedef BOOL (WINAPI *GPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

                HMODULE K32 = ::LoadLibraryW(L"KERNEL32");
                if (K32) {
                    GPI gpi = reinterpret_cast<GPI>(::GetProcAddress(K32, "GetProductInfo"));
                    if (NULL != gpi) {
                        gpi(osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &ptype);
                    }
                    ::FreeLibrary(K32);
                }

                switch (ptype)
                {
                case PRODUCT_ULTIMATE:                      temp << L"Ultimate Edition"; break;
                case PRODUCT_PROFESSIONAL:                  temp << L"Professional"; break;
                case PRODUCT_HOME_PREMIUM:                  temp << L"Home Premium Edition"; break;
                case PRODUCT_HOME_BASIC:                    temp << L"Home Basic Edition"; break;
                case PRODUCT_ENTERPRISE:                    temp << L"Enterprise Edition"; break;
                case PRODUCT_BUSINESS:                      temp << L"Business Edition"; break;
                case PRODUCT_STARTER:                       temp << L"Starter Edition"; break;
                case PRODUCT_CLUSTER_SERVER:                temp << L"Cluster Server Edition"; break;
                case PRODUCT_DATACENTER_SERVER:             temp << L"Datacenter Edition"; break;
                case PRODUCT_DATACENTER_SERVER_CORE:        temp << L"Datacenter Edition (core installation)"; break;
                case PRODUCT_ENTERPRISE_SERVER:             temp << L"Enterprise Edition"; break;
                case PRODUCT_ENTERPRISE_SERVER_CORE:        temp << L"Enterprise Edition (core installation)"; break;
                case PRODUCT_ENTERPRISE_SERVER_IA64:        temp << L"Enterprise Edition for Itanium-based Systems"; break;
                case PRODUCT_SMALLBUSINESS_SERVER:          temp << L"Small Business Server"; break;
                case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:  temp << L"Small Business Server Premium Edition"; break;
                case PRODUCT_STANDARD_SERVER:               temp << L"Standard Edition"; break;
                case PRODUCT_STANDARD_SERVER_CORE:          temp << L"Standard Edition (core installation)"; break;
                case PRODUCT_WEB_SERVER:                    temp << L"Web Server Edition"; break;
                }
            }

            if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
            {
                if (GetSystemMetrics(89 /*SM_SERVERR2*/))               temp << L"Server 2003 R2, ";
                else if (osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER)    temp << L"Storage Server 2003";
                else if (osvi.wSuiteMask & VER_SUITE_WH_SERVER )        temp << L"Home Server";
                else if( osvi.wProductType == VER_NT_WORKSTATION &&
                    si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
                    temp << L"XP Professional x64 Edition";
                else 
                    temp << L"Server 2003, ";

                // Test for the server type.
                if ( osvi.wProductType != VER_NT_WORKSTATION )
                {
                    if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64 )
                    {
                        if (osvi.wSuiteMask & VER_SUITE_DATACENTER)     
                            temp << L"Datacenter Edition for Itanium-based Systems";
                        else if(osvi.wSuiteMask & VER_SUITE_ENTERPRISE) 
                            temp << L"Enterprise Edition for Itanium-based Systems";
                    }

                    else if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
                    {
                        if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                            temp << L"Datacenter x64 Edition";
                        else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                            temp << L"Enterprise x64 Edition";
                        else 
                            temp << L"Standard x64 Edition";
                    }

                    else
                    {
                        if ( osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER )
                            temp << L"Compute Cluster Edition";
                        else if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                            temp << L"Datacenter Edition";
                        else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                            temp << L"Enterprise Edition";
                        else if ( osvi.wSuiteMask & VER_SUITE_BLADE )
                            temp << L"Web Edition";
                        else 
                            temp << L"Standard Edition";
                    }
                }
            }

            if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
            {
                temp << L"XP ";

                if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
                    temp <<  L"Home Edition";
                else 
                    temp <<  L"Professional";
            }

            if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
            {
                temp << L"2000 ";

                if ( osvi.wProductType == VER_NT_WORKSTATION )
                {
                    temp << L"Professional";
                }
                else 
                {
                    if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                        temp << L"Datacenter Server";
                    else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                        temp << L"Advanced Server";
                    else 
                        temp << L"Server";
                }
            }

            InfoStore::String csdVer(osvi.szCSDVersion);
            if (!csdVer.empty())
                temp << L" " << csdVer.c_str();

            temp << L" (build " << osvi.dwBuildNumber << L")";

            if (osvi.dwMajorVersion >= 6) {
                if (PROCESSOR_ARCHITECTURE_AMD64 == si.wProcessorArchitecture) {
                    temp << L", 64-bit";
                }
                else if (PROCESSOR_ARCHITECTURE_INTEL == si.wProcessorArchitecture) {
                    temp << L", 32-bit";
                }
            }
        }
        else {  
            temp << L"(some old version, 9x). You shouldn't see this string in normal product.";
        }
#endif
        return temp.str();
    }

    void InfoStore::SystemInfo::Initialize()
    {
#ifdef _WIN32
        SYSTEM_INFO si = {};
#  if _WIN32_WINNT >= 0x0501
        ::GetNativeSystemInfo(&si);
#  else
        ::GetSystemInfo(&si);
#  endif
        this->CpuType = si.dwProcessorType;
        this->CpuNum = si.dwNumberOfProcessors;
        this->PageSize = si.dwPageSize;
        this->AllocationGranularity = si.dwAllocationGranularity;

        HRESULT          hr = 0;
        OSVERSIONINFOEXW vi = { 0 };
        vi.dwOSVersionInfoSize = sizeof(vi);
        if (!::GetVersionExW((LPOSVERSIONINFOW)&vi)) {
            hr = static_cast<HRESULT>(::GetLastError());
            const auto errText = Str::ErrorCode<>::SystemMessage(hr);
            DH::TPrintf(L"OSVRSION: init failed %d `%s`\n", hr, errText.GetString());
        }
        this->Version.Major = vi.dwMajorVersion;
        this->Version.Minor = vi.dwMinorVersion;
        this->Version.DisplayName = GetOsDisplayString(vi, si);
#endif
    }
}
