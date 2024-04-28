#pragma once

#include "file.operation.types.h"
#include <filesystem>

namespace Twins
{
    class FileProgressDialog;

    class OperationBase
    {
    public:
        struct Params;

        virtual ~OperationBase();
        OperationBase(std::wstring const& path, std::wstring const& threadName);

        void Process(FileList files, Params& p) const;
        bool ProcessOne(Fl::Entry const& entry, Fs::path const& path, Params& p) const;
        void UpdateEntryProgress(FileSize transfered, Params& p) const;
        void IncrementTotal(FileSize v, Params& p) const;

        virtual void OnProcessEntryStart(Fl::Entry const& entry, Fs::path& destpath, Fs::path const& path, Params& p) const;
        virtual void ProcessEntry(Fl::Entry const& source, Fs::path const& destpath, Params& p) const = 0;
        virtual bool OnParseDirectoryStart(Fl::Entry const& source, Fs::path const& destpath, Params& p) const;
        virtual void OnParseDirectoryDone(Fl::Entry const& source, Fs::path const& destpath, Params& p) const;

    protected:
        Fs::path Destpath;
        std::wstring ThreadName;
        mutable FileSize TotalCount;
        mutable FileSize TotalSize;
        mutable FileSize ProcessedCount;
        mutable FileSize CurrentProcessedSize;
        mutable FileSize ProcessedSize;

        bool OnEntry(Fl::Entry const& source, Fs::path const& destpath, Params& p) const;
        virtual bool OnDirectory(Fl::Entry const& source, Fs::path const& destpath, Params& p) const;
    };

    struct OperationBase::Params
    {
        OperationBase const& Owner;
        SpecFlags& Flags;
        OperationResult& Error;
        FileProgressDialog& Ui;

        Params(OperationBase const& owner, SpecFlags& flags, OperationResult& error, FileProgressDialog& ui);
    };

    DWORD WINAPI _ProgressCb(LARGE_INTEGER, LARGE_INTEGER, LARGE_INTEGER, LARGE_INTEGER, DWORD, DWORD, HANDLE, HANDLE, LPVOID);
}
