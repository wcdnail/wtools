#pragma once

#include <map>
#include <atlstr.h>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace Twins
{
    struct DirectoryNotify: boost::noncopyable
    {
        int FirstAction;
        int LastAction;
        bool Added;
        bool Removed;
        bool Modified;
        bool RenameBegin;
        bool RenameDone;
        CString Filename;

        DirectoryNotify(PFILE_NOTIFY_INFORMATION ninfo, CString const& filename);

        void UpdateFlags(PFILE_NOTIFY_INFORMATION ninfo);
        CString ToString() const;
        bool Valid() const;

        static CString GetFilename(PFILE_NOTIFY_INFORMATION ninfo);
    };

    typedef std::shared_ptr<DirectoryNotify> DirectoryNotifyPtr;
    typedef std::map<CString, DirectoryNotifyPtr> DirectoryNotifyMap;
}
