#pragma once

#if 0
#include "fs.enum.h"
#include "record.info.list.h"

namespace Twins
{
    class ViewStatus;

    struct FSEnumerator: public Fs::Enumerator
    {
        FSEnumerator(bool showHiden, volatile LONG& stop, ViewStatus& status);

        virtual void operator() (PCTSTR path, PCTSTR dir, WIN32_FIND_DATA const& data);
        virtual bool Stop() const;

        RecordList const& GetCached() const;

    private:
        volatile LONG& stop_;
        bool showHidden_;
        RecordList cache_;
        ViewStatus& status_;
    };

    inline RecordList const& FSEnumerator::GetCached() const
    {
        return cache_;
    }
}
#endif
