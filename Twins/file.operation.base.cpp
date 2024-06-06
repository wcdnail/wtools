#include "stdafx.h"
#include "file.operation.base.h"
#include "file.list.h"
#include "dialogz.file.progress.h"
#include <twins.langs/twins.lang.strings.h>
#include <dh.tracing.h>
#include <boost/filesystem/operations.hpp>

namespace Twins
{
    OperationBase::~OperationBase() 
    {}

    OperationBase::OperationBase(std::wstring const& path, std::wstring const& threadName)
        : Destpath(path)
        , ThreadName(threadName)
        , TotalCount(0)
        , TotalSize(0)
        , ProcessedCount(0)
        , ProcessedSize(0)
    {}

    OperationBase::Params::Params(OperationBase const& owner, SpecFlags& flags, OperationResult& error, FileProgressDialog& ui)
        : Owner(owner)
        , Flags(flags)
        , Error(error)
        , Ui(ui)
    {}

    void OperationBase::Process(FileList files, Params& p) const
    {
        DH::ScopedThreadLog lg(ThreadName.c_str());

        TotalCount = files.Count();
        TotalSize = files.SumSize();
        p.Ui.OnStart(TotalCount, TotalSize);

        for (Fl::List::const_iterator it = files.Begin(); it != files.End(); ++it)
        {
            if (!ProcessOne(*it, Destpath, p))
                break;
        }

        p.Ui.OnStop(TotalCount, TotalSize, ProcessedCount);
        p.Ui.Close(p.Error.value());
    }

    void OperationBase::OnProcessEntryStart(Fl::Entry const& entry, Fs::path& destpath, Fs::path const& path, Params& p) const
    {
        destpath = path;
        destpath /= entry.GetFilename();

        p.Ui.SetSource(entry.GetPath());
        p.Ui.SetDest(destpath.native());
    }

    bool OperationBase::ProcessOne(Fl::Entry const& entry, Fs::path const& path, Params& p) const
    {
        if (p.Flags.IsCanceled() || p.Ui.IsCanceled())
        {
            p.Error.assign(ERROR_CANCELLED, p.Error.category());
            return false;
        }

        Fs::path destpath;
        OnProcessEntryStart(entry, destpath, path, p);

        p.Ui.Current().Range(0, entry.GetSize());
        p.Ui.Current() = 0;

        if (!OnEntry(entry, destpath, p))
            return false;

        ProcessedSize += entry.GetSize();
        p.Ui.OnCurrentProgressDone(++ProcessedCount, TotalCount, ProcessedSize, TotalSize);

        ::Sleep(1);
        return true;
    }

    void OperationBase::UpdateEntryProgress(FileSize transfered, Params& p) const
    {
        p.Ui.OnCurrentProgress(transfered, ProcessedSize + transfered, TotalSize);
    }

    bool OperationBase::OnEntry(Fl::Entry const& source, Fs::path const& destpath, Params& p) const
    {
        if (source.IsDir())
            return OnDirectory(source, destpath, p);

        Fl::Entry dest(destpath.c_str(), true);
        if (dest.IsValid() && p.Flags.Check(SpecFlags::AskUser))
        {
            p.Ui.ShowFileReplaceDialog(source, dest, p.Flags, true);

            if (p.Flags.IsCanceled())
                return false;

            else if (p.Flags.IsSkipped())
                return true;

            p.Flags.CheckSilent();

// ##TODO: Check other flags"))
        }

        ProcessEntry(source, destpath, p);

        DH::TPrintf(0, L"PrcEntry: (%s) - %d\n", source.GetPath().c_str(), p.Error.value());
        return ((p.Error && p.Flags.Check(SpecFlags::SkipError)) ? true : !p.Error);
    }

    class DirList: Fl::List
    {
    private:
        typedef Fl::List Super;

    public:
        DirList(OperationBase const& operation, Fs::path const& destpath, OperationBase::Params& params)
            : Super()
            , Operation(operation)
            , Destpath(destpath)
            , p(params)
        {}

        using Super::Load;
        using Super::Count;

    private:
        OperationBase const& Operation;
        Fs::path const& Destpath;
        OperationBase::Params& p;

        virtual void OnEntry(Fl::Entry const& entry);
    };

    void OperationBase::IncrementTotal(FileSize v, Params& p) const
    {
        ++TotalCount;
        TotalSize += v;

        p.Ui.Total().Range(0, TotalSize);
    }

    void DirList::OnEntry(Fl::Entry const& entry)
    {
        if (p.Ui.IsCanceled())
        {
            p.Error.assign(ERROR_CANCELLED, p.Error.category());
            //ec = p.Error;
        }
        else
        {
            p.Owner.IncrementTotal(entry.GetSize(), p);
            Operation.ProcessOne(entry, Destpath, p);

            //if (!Operation.ProcessOne(entry, Destpath, p))
                //ec = p.Error ? p.Error : Fl::Error(ERROR_ACCESS_DENIED, p.Error.category());
        }
    }

    bool OperationBase::OnDirectory(Fl::Entry const& source, Fs::path const& destpath, Params& p) const
    {
        DH::TPrintf(0, L"PrcFoldr: (%s) BEGIN\n", source.GetPath().c_str());

        if (OnParseDirectoryStart(source, destpath, p))
        {
            ++TotalCount;
            p.Ui.OnStart(TotalCount, TotalSize);

            DirList dirlist(*this, destpath, p);
            Enumerator::LoadArgs args(source.GetPath(), L"*", false, true, false);
            dirlist.Load(args);

            p.Ui.OnStop(TotalCount, TotalSize, ProcessedCount);
            OnParseDirectoryDone(source, destpath, p);
        }

        DH::TPrintf(0, L"PrcFoldr: (%s) END\n", source.GetPath().c_str());
        return ((p.Error && p.Flags.Check(SpecFlags::SkipError)) ? true : !p.Error);
    }

    DWORD WINAPI _ProgressCb(LARGE_INTEGER /*totalSize*/
                           , LARGE_INTEGER totalTransferred
                           , LARGE_INTEGER /*streamSize*/
                           , LARGE_INTEGER /*streamTransferred*/
                           , DWORD /*streamNumber*/
                           , DWORD /*callbackReason*/
                           , HANDLE /*sourceFile*/
                           , HANDLE /*destinationFile*/
                           , LPVOID pointee)
    {
        OperationBase::Params* p = reinterpret_cast<OperationBase::Params*>(pointee);

        if (NULL == p)
            return PROGRESS_STOP;

        p->Owner.UpdateEntryProgress(totalTransferred.QuadPart, *p);
        return p->Ui.IsCanceled() ? PROGRESS_CANCEL : PROGRESS_CONTINUE;
    }

    void OperationBase::ProcessEntry(Fl::Entry const& source, Fs::path const& destpath, Params& p) const {}
    bool OperationBase::OnParseDirectoryStart(Fl::Entry const& source, Fs::path const& destpath, Params& p) const { return true; }
    void OperationBase::OnParseDirectoryDone(Fl::Entry const& source, Fs::path const& destpath, Params& p) const {}
}
