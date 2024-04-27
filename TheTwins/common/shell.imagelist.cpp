#include "stdafx.h"
#include "shell.imagelist.h"
#include <crash.report/exception.h>
#include <shlobj.h>

#pragma comment(lib, "shell32")

namespace Sh
{
    static HIMAGELIST _GetShellImageList(UINT flags) /* throw(std::runtime_error) */
    {
        flags |= SHGFI_LINKOVERLAY;
        flags |= SHGFI_OPENICON;

        TCHAR sysRoot[MAX_PATH] = {0};
        if (!::SHGetSpecialFolderPath(NULL, sysRoot, CSIDL_SYSTEM, FALSE))
            throw CrashException("Не могу получить путь к корню системы", ::GetLastError());
        
        sysRoot[3] = 0;
        SHFILEINFO info = {0};
        HIMAGELIST result = (HIMAGELIST)::SHGetFileInfo(sysRoot, 0, &info, sizeof(info), flags);

        if (!result)
            throw CrashException("Не могу загрузить список иконок оболочки", ::GetLastError());

        return result;
    }

    Imagelist::Imagelist(bool useSmallIcons)
        : Handle(_GetShellImageList(SHGFI_SYSICONINDEX | (useSmallIcons ? SHGFI_SMALLICON : SHGFI_LARGEICON)))
        , Icons()
    {}

    Imagelist::~Imagelist()
    {
        Clear();
    }

    void Imagelist::Clear()
    {
        if (!Icons.empty())
        {
            for (IconMap::const_iterator it = Icons.cbegin(); it != Icons.cend(); ++it)
                ::DestroyIcon(it->second);

            Icons.clear();
        }

        if (NULL != Handle)
        {
            ::ImageList_Destroy(Handle);
            Handle = NULL;
        }
    }
}
