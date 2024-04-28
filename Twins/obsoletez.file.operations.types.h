#pragma once

#if 0
#include <atlstr.h>
#include <vector>

namespace Twins
{
    namespace Fo // File operations...
    {
        typedef std::vector<CString> StringArray;

        CString ToString(StringArray const& files, CString const& separator, int maximal = -1);
        CString ToZzString(StringArray const& files, int buffersize);
    }
}
#endif
