#pragma once

#include "wcdafx.api.h"
#include <iosfwd>
#include <string>
#include <sstream>

namespace Runtime
{
    enum Endianness
    {
        UnknownEndian = -1,
        LittleEndian,
        BigEndian,
    };

    struct InfoStore;

    WCDAFX_API InfoStore const& Info();

    struct InfoStore
    {
        typedef std::wstring String;
        typedef std::wostream OStream;
        typedef std::wostringstream StringStream;

        struct SystemInfo
        {
            struct VersionInfo
            {
                DWORD        Major;
                DWORD        Minor;
                String DisplayName;

            private:
                friend SystemInfo;
                ~VersionInfo();
                VersionInfo();
            };

            unsigned               CpuType;
            unsigned                CpuNum;
            unsigned              PageSize;
            unsigned AllocationGranularity;
            String           RootDirectory;
            String           HomeDirectory;
            VersionInfo            Version;

            void Initialize();

            static Endianness GetEndian();
            static wchar_t const* GetEndianString();

        private:
            friend InfoStore;
            SystemInfo();
            ~SystemInfo();
        };

        struct ExecutableInfo
        {
            struct VersionInfo
            {
                String ProductName;
                String ProductVersion;
                String FileDescription;
                String FileVersion;
                String Comments;
                String InternalName;
                String CompanyName;
                String LegalCopyright;
                String LegalTrademarks;
                String PrivateBuild;
                String OriginalFilename;
                String SpecialBuild;

            private:
                friend ExecutableInfo;

                VersionInfo();
                ~VersionInfo();
            };

            String Directory;
            String Filename;
            VersionInfo Version;

        private:
            friend InfoStore;

            ExecutableInfo();
            ~ExecutableInfo();
        };

        struct HostInfo
        {
            String Name;

        private:
            friend InfoStore;

            HostInfo();
            ~HostInfo();
        };

        struct UserInfo
        {
            String Name;

        private:
            friend InfoStore;

            UserInfo();
            ~UserInfo();
        };

        static WCDAFX_API wchar_t const* Compiler;
        static WCDAFX_API wchar_t const* CLib;
        static WCDAFX_API wchar_t const* BoostVersion;
        static WCDAFX_API wchar_t const* TargetOs;

        SystemInfo System;
        ExecutableInfo Executable;
        HostInfo Host;
        UserInfo User;

        WCDAFX_API void SimpleReport(OStream& stream) const;
        WCDAFX_API String SimpleReport() const;

    private:
        friend WCDAFX_API InfoStore const& Info();

        InfoStore();
        ~InfoStore();
    };
}
