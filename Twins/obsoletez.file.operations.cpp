#include "stdafx.h"
#include "file.operations.h"
#include "panel.h"
#include "dialogz.copymove.prompt.h"
#include "dialogz.fo.progress.h"
#include "dialogz.fo.confirm.h"
#include "fs.enum.h"
#include <dh.tracing.h>
#include <locale.helper.h>
#include <string.utils.error.code.h>
#include <atlctrls.h>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_array.hpp>

namespace Twins
{
#if 0
    namespace Fo // File operations...
    {
        class Executor: boost::noncopyable
        {
        public:
            typedef boost::function<HRESULT (CString const&, CString const&, Executor*)> OperationFunc;

            enum 
            {
                opCopy = 5,
                opMove = 6,
                opToTrash = 8,
                opDelete = 9,
            };

            enum 
            {
                Runing = -1,
                Canceled = 0,
                Paused = 1,
                EventCount
            };

            Executor(StringArray const& entries, int opType)
                : exit_(::CreateEvent(NULL, FALSE, FALSE, NULL), CloseHandle)
                , paused_(::CreateEvent(NULL, FALSE, FALSE, NULL), CloseHandle)
                , progressUi_(GetCaption(opType))
                , destDir_()
                , destPathname_()
                , entries_(entries)
                , canceled_(FALSE)
                , opType_(opType)
            {}

            ~Executor()
            {}

            int GetOperationType() const { return opType_; }

        private:
            static CString GetCaption(int opType)
            {
                switch (opType)
                {
                case opCopy:    return _LS("Копирование файлов");
                case opMove:    return _LS("Перемещение файлов");
                case opToTrash: return _LS("Удаление файлов в корзину");
                case opDelete:  return _LS("Удаление файлов");
                }

                return _LS("Неизвестная операция");
            }

            static CString GetOperationPrefix(int opType)
            {
                switch (opType)
                {
                case opCopy:    return _LS("Копировать ");
                case opMove:    return _LS("Переместить ");
                case opToTrash: return _LS("Удалить в корзину");
                case opDelete:  return _LS("Удалить ");
                }

                return L"???";
            }

        public:
            bool Confirm(PCWSTR question)
            {
                ConfirmDialog dlg(GetCaption(opType_), question);
                dlg.GetListFrom(entries_);
                int rv = (int)dlg.DoModal();
                // TODO: CommonDialog::ResultYesToAll == rv - don't ask again
                return (CommonDialog::ResultYes == rv) || (CommonDialog::ResultYesToAll == rv);
            }

            HRESULT Start(OperationFunc const& operation, HWND parent)
            {
                progressUi_.SetThreadFn(boost::bind(&Executor::ThreadProc, this, operation));

                UINT_PTR dr = progressUi_.DoModal(parent);
                if (CommonDialog::ResultCancel == dr)
                    Stop();

                progressUi_.GetThread().join();
                return progressUi_.GetErrorCode();
            }

        private:
            CString GetOperationPrompt() const
            {
                CString rv = GetOperationPrefix(opType_);
                int count = (int)entries_.size();

                if (count == 1)
                    rv.AppendFormat(L"`%s`", entries_[0]);
                else
                {
                    int m = count % 10;
                    rv.AppendFormat(L"%d %s", count, (m == 1 ? _LS(" файл") : ((m > 1 && m < 5) ? _LS(" файла") : _LS(" файлов"))));
                }

                rv += _LS(" в");
                return rv;
            }

        public:
            bool Prepare(CString const& destDir, HWND parent, unsigned flags)
            {
                destDir_ = destDir;
                destPathname_ = Fs::LinkFilename(destDir_, (entries_.size() > 1 ? L"*" : Fs::GetFilename(entries_[0])));

                if (0 != (NoUiPrompt & flags))
                    return true;

                CopyMovePromptDialog dlg(GetCaption(opType_), GetOperationPrompt(), destPathname_);
                UINT_PTR dr = dlg.DoModal(parent);

                return CommonDialog::ResultStart == dr;
            }

            BOOL* GetCanceledPtr() { return &canceled_; }
            ProgressDialog& GetProgress() { return progressUi_; }

            int WaitState(DWORD timeout = 10) const
            {
                HANDLE events[] = { exit_.get(), paused_.get() };
                int state = (int)::WaitForMultipleObjects(_countof(events), events, FALSE, timeout);

                if (Paused != state || Canceled != state)
                    state = Runing;

                return state;
            }

        private:
            boost::shared_ptr<void> exit_;
            boost::shared_ptr<void> paused_;
            ProgressDialog progressUi_;
            CString destDir_;
            CString destPathname_;
            StringArray const& entries_;
            BOOL canceled_;
            int opType_;

            void Stop()
            {
                ::SetEvent(exit_.get());
                canceled_ = TRUE;
            }

            void Close(int rv) 
            {
                GetProgress().Close(rv);
            }

            void OnPause() 
            {
                /* TODO: pause copying */ 
            }

            bool GetSourceInfo(CString const& pathname, CString& source, DWORD& sourceAttrs, Fs::SizeType& sourceSize) const
            {
                if (Fs::GetFileAttributesAndSize(pathname, sourceAttrs, sourceSize))
                    source = pathname;

                return !source.IsEmpty();
            }

            void CheckDest(CString const& dest)
            {
                //if (Fs::GetFileAttributesAndSize(pathname, sourceAttrs, sourceSize))
            }

            void ThreadProc(OperationFunc const& operation)
            {
                Dh::ScopedThreadLog lg(0, L"FileOper: `%s`", GetCaption(opType_));

                HRESULT hr = S_OK;
                int count = (int)entries_.size();

                GetProgress().SetTotalRange(0, count);

                int counter = 0;
                bool multiple = count > 1;
                for (StringArray::const_iterator it = entries_.begin(); it != entries_.end(); ++it)
                {
                    if (Canceled == WaitState())
                    {
                        hr = ERROR_CANCELLED;
                        canceled_ = TRUE;
                        break;
                    }

                    CString source;
                    DWORD attrs = 0;
                    Fs::SizeType size = 0;

                    if (!GetSourceInfo(*it, source, attrs, size))
                    {
                        HRESULT hr = ::GetLastError();
                        Dh::ThreadPrintf(L"NoSource: %08x - `%s`\n", hr, *it);
                        continue;
                    }

                    CString name = Fs::GetFilename(source);
                    CString dest = (multiple ? Fs::LinkFilename(destDir_, name) : destPathname_);

                    GetProgress().SetHints(source, dest);
                    GetProgress().SetCurrentRange<RecordInfo::SizeType>(0, size);

                    GetProgress().SetTotal(++counter);

                    CheckDest(dest);

                    hr = operation(boost::cref(source), boost::cref(dest), this);

                    // FIXME: log
                    Dh::ThreadPrintf(L"FExecute: `%s` - %x %s\n", source, hr
                        , (0 != hr ? Str::ErrorCode<wchar_t>::SystemMessage(hr) : L""));

                    // TODO: break on error...
                }

                GetProgress().SetTotal(counter);
                GetProgress().SetErrorCode(hr);

                Close(canceled_ ? CommonDialog::ResultCancel : CommonDialog::ResultOk);
            }
        };

        static DWORD WINAPI ProgressFunc(LARGE_INTEGER /*totalSize*/
                                       , LARGE_INTEGER totalTransferred
                                       , LARGE_INTEGER /*streamSize*/
                                       , LARGE_INTEGER /*streamTransferred*/
                                       , DWORD /*streamNumber*/
                                       , DWORD /*callbackReason*/
                                       , HANDLE /*sourceFile*/
                                       , HANDLE /*destinationFile*/
                                       , LPVOID pointee
                                       )
        {
            Executor* executor = (Executor*)pointee;
            executor->GetProgress().SetCurrent<RecordInfo::SizeType>(totalTransferred.QuadPart);

            int state = executor->WaitState(5);
            if (Executor::Canceled == state)
                return PROGRESS_CANCEL;

            return Executor::Paused == state ? PROGRESS_STOP : PROGRESS_CONTINUE;
        }


        static HRESULT CopyOperation(CString const& source, CString const& dest, Executor* executor)
        {
            BOOL rv = ::CopyFileExW(source, dest, ProgressFunc, (PVOID)executor, executor->GetCanceledPtr(), COPY_FILE_FAIL_IF_EXISTS);
            HRESULT hr = ::GetLastError();
            return rv ? S_OK : hr;
        }

#if 0
        static HRESULT MoveOperation(CString const& srcPathname
                                   , CString const& dstPathname
                                   , RecordInfo const& info
                                   , Executor* executor)
        {
            BOOL rv = ::MoveFileWithProgressW(srcPathname, dstPathname
                                            , ProgressFunc
                                            , (PVOID)executor
                                            , MOVEFILE_WRITE_THROUGH
                                            );

            HRESULT hr = ::GetLastError();
            return rv ? S_OK : hr;
        }


        static HRESULT MoveToTrash(CString const& srcPathname
                                 , CString const& dstPathname
                                 , RecordInfo const& info
                                 , Executor* executor)
        {
            CString zzBuffer(L'\0', srcPathname.GetLength() + 2);
            zzBuffer.SetString(srcPathname);

            SHFILEOPSTRUCTW opfo = {0};

            opfo.wFunc  = FO_DELETE;
            opfo.pFrom  = zzBuffer;
            opfo.fFlags = FOF_NOCONFIRMATION 
                        | FOF_NOERRORUI 
                        | FOF_SILENT 
                        | (Executor::opToTrash == executor->GetOperationType() ? FOF_ALLOWUNDO : 0);

            HRESULT hr = ::SHFileOperationW(&opfo);

            executor->GetProgress().SetCurrent<RecordInfo::SizeType>(info.fileSize);

            return hr;
        }

        template <class Operation>
        HRESULT PerformOperation(Panel& source, Panel& destination, int opType, Operation const& operation, HWND mainFrame)
        {
            RecordList entries;
            if (!source.GetSelection(entries))
                return ERROR_INVALID_PARAMETER;

            Executor executor(entries, opType);
            if (!executor.Prompt(destination.GetDirectoryManager().GetPathName().c_str(), mainFrame))
                return ERROR_CANCELLED;

            HRESULT hr = executor.Start(operation, mainFrame);
            if (S_OK == hr)
                source.ClearSelection();

            return hr;
        }
#endif

        HRESULT PerformMove(Panel& source, Panel& destination, HWND mainFrame /*= NULL*/)
        {
            //HRESULT hr = PerformOperation(source, destination, Executor::opMove, MoveOperation, mainFrame);
            //return hr;
            return E_NOTIMPL;

        }

        HRESULT PerformDelete(Panel& source, bool toTrash, HWND mainFrame /*= NULL*/)
        {
            //RecordList entries;
            //if (!source.GetSelection(entries))
            //    return ERROR_INVALID_PARAMETER;

            //Executor executor(entries, toTrash ? Executor::opToTrash : Executor::opDelete);
            //if (!executor.Confirm(_LS("Действительно хотите удалить?")))
            //    return ERROR_CANCELLED;

            //HRESULT hr = executor.Start(MoveToTrash, mainFrame);
            //return hr;
            return E_NOTIMPL;
        }

        HRESULT CopyFiles(StringArray const& files, Panel& destination, unsigned flags /*= Normal*/, HWND parent /*= NULL*/)
        {
            HRESULT hr = ERROR_INVALID_STATE;

            Executor op(files, Executor::opCopy);
            if (op.Prepare(destination.GetDirectoryManager().GetPathName().c_str(), parent, flags))
                hr = op.Start(CopyOperation, parent);

            return hr;
        }

        HRESULT CopyFiles(Panel& source, Panel& destination, unsigned flags /*= Normal*/, HWND parent /*= NULL*/)
        {
            HRESULT hr = ERROR_INVALID_PARAMETER;

            StringArray files;
            if (source.GetSelection(files))
            {
                hr = CopyFiles(files, destination, flags, parent);
                source.ClearSelection();
            }

            return hr;
        }
    }
#endif
}

