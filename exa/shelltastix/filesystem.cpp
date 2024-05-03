#include "stdafx.h"
#include "filesystem.h"
#include "cp.internal.commands.h"
#include "console.view.h"
#include "execution.state.h"
#include <locale.helper.h>
#include <string.utils.error.code.h>
#include <dh.timer.h>
#include <boost/bind.hpp>
#include <algorithm>
#include <exception>

namespace Shelltastix
{
    static inline Filesystem::String PathStr(Filesystem::Path const& p, bool posix = true)
    {
        return posix ? p.generic_wstring() : p.wstring();
    }

    Filesystem::Filesystem(Console::View& console, bool& posix)
        : Console(console)
        , Posix(posix)
        , envPaths_()
    {
        SetupCommands();
    }

    Filesystem::~Filesystem()
    {}

    Filesystem::String Filesystem::GetCurrentPath(bool posix/* = true*/)
    {
        return PathStr(boost::filesystem::current_path(), posix);
    }

    void Filesystem::WriteResult(PCWSTR operation, Path const& p, Dh::Timer const& timer, HRESULT result) const
    {
        if (Console.IsVerboseOutput())
        {
            Console.WriteLine(L"%s `%s` %.10f %s", operation, PathStr(p, Posix).c_str(), timer.Seconds(), _LS("сек."));

            if (0 != result)
                Console.WriteLine(L"%d `%s`", result, (wchar_t const*)Str::ErrorCode<wchar_t>::SystemMessage(result));
        }
    }

    void Filesystem::Pwd() const
    {
        Console.WriteLine(GetCurrentPath(Posix));
    }

    bool Filesystem::IsPathExist(Path const& p) const
    {
        bool rv = boost::filesystem::exists(p);
        if (!rv)
            Console.WriteLine(L"`%s` - %s", PathStr(p, Posix).c_str(), _LS("путь не существует"));
        return rv;
    }

    bool Filesystem::IsDir(Path const& p) const
    {
        bool rv = boost::filesystem::is_directory(p);
        if (!rv)
            Console.WriteLine(L"`%s` - %s", PathStr(p, Posix).c_str(), _LS("не является директорией"));
        return rv;
    }

    void ExpandUserPath(Filesystem::String& directory) 
    {
        if (!directory.empty() && directory[0] == L'~') 
        {
            wchar_t const* home = ::_wgetenv(L"HOME");

            if (!home)
                home = ::_wgetenv(L"USERPROFILE");

            if (home)
            {
                directory.replace(0, 1, home);
            }
            else 
            {
                wchar_t const* hdrive = ::_wgetenv(L"HOMEDRIVE");
                wchar_t const* hpath = ::_wgetenv(L"HOMEPATH");

                if (hdrive && hpath)
                    directory.replace(0, 1, Filesystem::String(hdrive) + hpath);
            }
        }
    }

    void Filesystem::Cd(String const& args) const
    {
        Dh::Timer timer;

        String directory = args.empty() ? L"~" : args;
        ExpandUserPath(directory);
        Path p(directory);

        if (!IsPathExist(p) || !IsDir(p))
            return ;

        BOOL rv = ::SetCurrentDirectoryW(p.make_preferred().wstring().c_str());
        HRESULT hr = ::GetLastError();
        WriteResult(_LS("cd"), p, timer, (rv ? 0 : hr));
    }

    template <class Iterator, class T>
    static void DirWalk(Filesystem::Path const& p, T& functor, HRESULT& hr)
    {
        typename Iterator begin(p);
        typename Iterator end;
        for (typename Iterator it = begin; it != end; ++it)
        {
            if (Execution().Canceled())
            {
                hr = ERROR_CANCELLED;
                break;
            }

            functor(*it);
        }
    }

    void Filesystem::WriteFileNameAndInfo(DirEntry const& entry) const
    {
        String displayString = entry.path().filename().generic_wstring();
        Console.WriteLine(L"%s", displayString.c_str());
    }

    void Filesystem::Ls(String const& args) const
    {
        Dh::Timer timer;
        HRESULT hr = 0;
        Path p;

        try
        {
            p = args.empty() ? boost::filesystem::current_path() : Path(args);
            if (IsPathExist(p) && IsDir(p))
            {
                DirWalk<DirIterator>(p, boost::bind(&Filesystem::WriteFileNameAndInfo, this, _1), hr);
                Console.NewLine();
            }
        }
        catch (std::exception const& ex)
        {
            hr = ::GetLastError();
            Console.WriteLine("\n%S: %s\n", _LS("Внутренняя ошибка"), ex.what());
        }

        WriteResult(_LS("ls"), p, timer, hr);
    }

    static bool ExecutableExists(Filesystem::Path const& p, Filesystem::String const& name, Filesystem::String& target, bool posix)
    {
        static wchar_t const* ext[] = { NULL, L".exe", L".com", L".bat", L".cmd" };

        for (size_t i=0; i<_countof(ext); i++)
        {
            Filesystem::String temp(name);

            if (ext[i])
                temp += ext[i];

            Filesystem::Path exePath = p;
            exePath /= temp;

            if (boost::filesystem::exists(exePath))
            {
                target = PathStr(exePath, posix);
                return true;
            }
        }

        return false;
    }

    bool Filesystem::LoadEnvPaths() const
    {
        PWSTR envp = ::_wgetenv(L"PATH");
        if (NULL != envp)
        {
            envPaths_ = envp;
        }
        else
        {
            HRESULT hr = ::GetLastError();

            ATLTRACE(L"FSEXEC: Can't get ENVIRONMENT $PATH!\n"
                        L"FSEXEC: Last error = %08x `%s`\n", hr, 
                        (PCWSTR)Str::ErrorCode<wchar_t>::SystemMessage(hr));
        }

        return !envPaths_.empty();
    }

    Filesystem::String Filesystem::ResolveExecutablePath(String const& name, bool posix/* = true*/) const
    {
        String temp;
        if (ExecutableExists(boost::filesystem::current_path(), name, temp, posix))
            return temp;

        if (!LoadEnvPaths())
            return name;

        String const& paths = envPaths_;
        String::size_type n = 0, beg = 0, len = 0;

        do
        {
            n = paths.find(L';', beg);

            len = String::npos == n ? paths.length() : n;

            String path = paths.substr(beg, len - beg);
            String temp;
            if (ExecutableExists(Path(path), name, temp, posix))
                return temp;

            beg = String::npos == n ? 0 : n + 1;
        }
        while (String::npos != n);

        return L"";
    }

    void Filesystem::SetupCommands()
    {
        Command::Internals().Add(Command::Def(L"ipwd", L"Вывод текущего пути (совместимость с POSIX).", boost::bind(&Filesystem::Pwd, this)));
        Command::Internals().Add(Command::Def(L"cd",   L"Смена текущей директории: cd [путь]", boost::bind(&Filesystem::Cd, this, _1)));
        Command::Internals().Add(Command::Def(L"ils",  L"Листинг содержимого директории: ls [путь]", boost::bind(&Filesystem::Ls, this, _1)));
    }
}
