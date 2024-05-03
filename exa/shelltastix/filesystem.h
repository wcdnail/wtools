#pragma once

#include "cp.command.h"
#include <dh.timer.h>
#include <boost/noncopyable.hpp>
#include <boost/filesystem.hpp>

namespace Shelltastix
{
    namespace Console { class View; }

    class Filesystem: boost::noncopyable
    {
    public:
        typedef Command::Def::String String;
        typedef boost::filesystem::path Path;
        typedef boost::filesystem::directory_entry DirEntry;
        typedef boost::filesystem::directory_iterator DirIterator;
                
        Filesystem(Console::View& console, bool& posix);
        ~Filesystem();

        bool IsPathExist(Path const& p) const;
        bool IsDir(Path const& p) const;

        void Pwd() const;
        void Cd(String const& pathString) const;
        void Ls(String const& pathString) const;

        static String GetCurrentPath(bool posix = true);
        String ResolveExecutablePath(String const& name, bool posix = true) const;

        void WriteResult(PCWSTR operation, 
                         Path const& p, 
                         Dh::Timer const& timer, 
                         HRESULT result = ::GetLastError()
                         ) const;

    private:
        mutable Console::View& Console;
        bool& Posix;
        mutable String envPaths_;

        void SetupCommands();
        void WriteFileNameAndInfo(DirEntry const& entry) const;
        bool LoadEnvPaths() const;
    };
}
