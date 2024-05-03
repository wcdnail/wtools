#pragma once

#include <string>

namespace Shelltastix
{
    class Filesystem;
    class RedirectStd;
    namespace Console { class View; }

    bool RunExternal(Console::View& con, Filesystem& fs, std::wstring const& name, std::wstring const& arguments = L"");
}
