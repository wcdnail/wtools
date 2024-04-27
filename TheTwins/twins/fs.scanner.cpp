#include "stdafx.h"
#if 0
#include "fs.scanner.h"
#include "panel.view.status.h"
#include <string.utils.human.size.h>
#include <atltime.h>

namespace Twins
{
    FSEnumerator::FSEnumerator(bool showHiden, volatile LONG& stop, ViewStatus& status)
        : stop_(stop)
        , showHidden_(showHiden)
        , cache_()
        , status_(status)
    {
        ::InterlockedExchange(&stop, 0);
    }
    
    void FSEnumerator::operator() (PCTSTR path, PCTSTR dir, WIN32_FIND_DATA const& data)
    {
        if (Fs::IsHidden(data))
        {
            if (!Fs::IsUpDir(data) && !showHidden_)
                return ;
        }

        Fs::EntryInfo ent(path, dir, data);

        CString fileName, fileType;
        Fs::Split(ent, fileName, fileType);

        RecordInfo info;
        info.name = fileName;
        info.type = fileType;
        info.attr = data.dwFileAttributes;
        info.pathName = ent.pathName;
        info.ResetIntType();

        Fs::UpdateFileTime(info, data);
        Fs::UpdateFileSize(info, ent.isDir, ent.size);

        cache_.PushBack(info);
        status_.Accumulate(info);
    }
    
    bool FSEnumerator::Stop() const 
    {
        // TODO: Исправить!!!
        LONG rv = ::InterlockedCompareExchange(&stop_, 2, 1);
        return 2 == rv; 
    }
}
#endif // 0
