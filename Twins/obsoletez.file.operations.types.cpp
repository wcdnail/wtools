#include "stdafx.h"
#if 0
#include "file.operations.types.h"
#include <dh.tracing.h>
#include <algorithm>

namespace Twins
{
    namespace Fo // File operations...
    {
#if 0
        boost::shared_array<ITEMIDLIST> ToIdList(StringArray const& files)
        {
            boost::shared_array<ITEMIDLIST> rv;

            if (!files.empty())
            {
                rv.reset(new ITEMIDLIST[files.size()]);
            }

            return rv;
        }
#endif

        CString ToString(StringArray const& files, CString const& separator, int maximal /*= -1*/)
        {
            if (1 == files.size())
                return files[0];

            CString result;
            int counter = 0;
            for (StringArray::const_iterator it = files.begin(); it != files.end(); ++it)
            {
                result += *it;
                result += separator; 

                counter++;
                if ((-1 != maximal) && (counter > maximal))
                    break;
            }

            return result;
        }

        CString ToZzString(StringArray const& files, int buffersize)
        {
            if (1 == files.size())
                return CString(files[0], files[0].GetLength() + 2);

            struct Result
            {
                Result(CString& string)
                    : string_(string)
                    , beg_(string.GetBuffer())
                    , len_(string.GetAllocLength() - 2)
                {
                    Dh::ThreadPrintf(L"ZzString: result buffer = %p\n", beg_);
                }

                void operator() (CString const& line)
                {
                    int cc = line.GetLength() + 1;
                    if (cc > 0)
                    {
                        CString::CopyChars(beg_, len_, line, cc);
                        beg_ += cc;
                    }
                }

                CString const& GetString() const { return string_; }

            private:
                CString& string_;
                PTSTR beg_;
                int len_;
            };

            CString result(_T('\0'), buffersize + 2);
            return std::for_each(files.begin(), files.end(), Result(result)).GetString();
        }
    }
}
#endif
