#include "stdafx.h"
#include "directory.watch.notify.h"
#include <dh.tracing.h>

namespace Twins
{
    static void DebugDump(DirectoryNotify& dn)
    {
#ifdef _DEBUG2
        DH::TPrintf(L"DwNotify: %s\n", dn.ToString());
#endif
    }

    enum
    {
        InvalidAction = -1,
    };

    DirectoryNotify::DirectoryNotify(PFILE_NOTIFY_INFORMATION ninfo, CString const& filename)
        : FirstAction(ninfo ? ninfo->Action : InvalidAction)
        , LastAction(InvalidAction)
        , Added(FILE_ACTION_ADDED == FirstAction)
        , Removed(FILE_ACTION_REMOVED == FirstAction)
        , Modified(FILE_ACTION_MODIFIED == FirstAction)
        , RenameBegin(FILE_ACTION_RENAMED_OLD_NAME == FirstAction)
        , RenameDone(FILE_ACTION_RENAMED_NEW_NAME == FirstAction)
        , Filename(filename)
    {
        DebugDump(*this);
    }

    void DirectoryNotify::UpdateFlags(PFILE_NOTIFY_INFORMATION ninfo)
    {
        if (NULL != ninfo)
        {
            LastAction = ninfo->Action;
            Added = (FILE_ACTION_ADDED == LastAction ? true : Added);
            Removed = (FILE_ACTION_REMOVED == LastAction ? true : Removed);
            Modified = (FILE_ACTION_MODIFIED == LastAction ? true : Modified);
            RenameBegin = (FILE_ACTION_RENAMED_OLD_NAME == LastAction ? true : RenameBegin);
            RenameDone = (FILE_ACTION_RENAMED_NEW_NAME == LastAction ? true : RenameDone);

            DebugDump(*this);
        }
    }

    CString DirectoryNotify::ToString() const
    {
        CString rv;
        rv.Format(L"%2d %2d `%s` %s%s%s%s%s"
            , FirstAction, LastAction
            , Filename
            , (Added       ? L"+" : L"")
            , (Removed     ? L"-" : L"")
            , (Modified    ? L"~" : L"")
            , (RenameBegin ? L"!" : L"")
            , (RenameDone  ? L"^" : L"")
            );

        return rv;
    }

    CString DirectoryNotify::GetFilename(PFILE_NOTIFY_INFORMATION ninfo)
    {
        return CString(ninfo->FileName, ninfo->FileNameLength / sizeof(wchar_t));
    }

    bool DirectoryNotify::Valid() const
    {
        return InvalidAction != FirstAction;
    }
}
