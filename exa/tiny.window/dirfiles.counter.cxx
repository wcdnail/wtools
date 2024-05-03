#include "stdafx.h"
#include <dh.tracing.h>
#include <string.utils.human.size.h>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_array.hpp>

static size_t GetFilesCountViaBoostFs(wchar_t const* pathStr)
{
    boost::filesystem::path p = pathStr;
    size_t count = 0;
    double elapsed = 0.;

    {
        Dh::Timer tm;
        count = std::distance(boost::filesystem::directory_iterator(p), boost::filesystem::directory_iterator());
        elapsed = tm.Seconds();
    }

    Dh::ThreadPrintf(L"`%s` #%d %2.8f", pathStr, count, elapsed);
    return count;
}

static size_t GetFilesCountViaWinApi(wchar_t const* pathStr, size_t bsize, bool output)
{
    Dh::Timer tm;

    HANDLE h = ::CreateFileW(pathStr
        , READ_CONTROL | GENERIC_READ
        , FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
        , NULL
        , OPEN_EXISTING
        , FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_DIRECTORY
        , NULL);

    HRESULT hr = ::GetLastError();

    if (INVALID_HANDLE_VALUE == h)
    {
        Dh::ThreadPrintf(L"`%s` cF failed 0x%x\n", pathStr, hr);
        return 0;
    }

    boost::shared_ptr<void> dir(h, CloseHandle);
    boost::scoped_array<BYTE> bytes(new BYTE[bsize]);

    Dh::ThreadPrintf(L"`%s` hA %.3fM\n", pathStr, bsize / (1024. * 1024.));
    
    BOOL br = ::GetFileInformationByHandleEx(h, FileIdBothDirectoryInfo, bytes.get(), (DWORD)bsize);
    hr = ::GetLastError();
    Dh::ThreadPrintf(L"`%s` gI %d 0x%x\n", pathStr, br, hr);

    if (!br || (0 != hr))
        return 0;        

    size_t count = 0;
    bool theLast = false;
    PFILE_ID_BOTH_DIR_INFO info = (PFILE_ID_BOTH_DIR_INFO)bytes.get();

    for (;;)
    {
        if (output)
        {
            std::wstring name(info->FileName, info->FileNameLength / sizeof(wchar_t));

            ::OutputDebugStringW(name.c_str());
            ::OutputDebugStringW(L"\n");
        }

        info = (PFILE_ID_BOTH_DIR_INFO)(((BYTE*)info) + info->NextEntryOffset);
        ++count;

        if (theLast)
            break;

        if (!info->NextEntryOffset)
            theLast = true;
    }

    size_t rsize = ((BYTE*)info) - bytes.get() + (sizeof(FILE_ID_BOTH_DIR_INFO) + info->FileNameLength - sizeof(wchar_t));

    Dh::ThreadPrintf(L"`%s` #%d %.3fM %2.8f", pathStr, count, rsize / (1024. * 1024.), tm.Seconds());
    return count;
}

static size_t ReadDir(wchar_t const* path, unsigned char* buffer, size_t bsize, bool output)
{
    Dh::Timer tm;

    HANDLE h = ::CreateFileW(path
        , READ_CONTROL | GENERIC_READ
        , FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
        , NULL
        , OPEN_EXISTING
        , FILE_FLAG_BACKUP_SEMANTICS | FILE_ATTRIBUTE_DIRECTORY
        , NULL);

    HRESULT hr = ::GetLastError();

    if (INVALID_HANDLE_VALUE == h)
    {
        Dh::ThreadPrintf(L"`%s` cF failed 0x%x\n", path, hr);
        return 0;
    }

    boost::shared_ptr<void> dir(h, CloseHandle);

    ULARGE_INTEGER dsize = {};
    ::GetFileSizeEx(h, (PLARGE_INTEGER)&dsize);

    BOOL br = ::GetFileInformationByHandleEx(h, FileIdBothDirectoryInfo, buffer, (DWORD)bsize);
    hr = ::GetLastError();

    if (!br || (0 != hr))
    {
        Dh::ThreadPrintf(L"`%s` gI failed - %d 0x%x\n", path, br, hr);
        return 0;        
    }

    size_t count = 0;
    bool theLast = false;
    PFILE_ID_BOTH_DIR_INFO info = (PFILE_ID_BOTH_DIR_INFO)buffer;

    if (output)
        Dh::Printf(L"# FileId ####### ################################################################################################################################ # EndOfFile #### # AllocSize #### # EaSize #######\n");

    for (;;)
    {
        if (output)
        {
            std::wstring name(info->FileName, info->FileNameLength / sizeof(wchar_t));

            Dh::Printf(L"%016I64x %-128s %16s %16s %16s\n"
                , info->FileId.QuadPart
                , name.c_str()
                , Str::HumanSize(info->EndOfFile.QuadPart).c_str()
                , Str::HumanSize(info->AllocationSize.QuadPart).c_str()
                , Str::HumanSize(info->EaSize).c_str()
                );
        }

        info = (PFILE_ID_BOTH_DIR_INFO)(((BYTE*)info) + info->NextEntryOffset);
        ++count;

        if (theLast)
            break;

        if (!info->NextEntryOffset)
            theLast = true;
    }

    size_t usize = (((unsigned char*)info) - buffer) + (sizeof(*info) + info->FileNameLength - sizeof(wchar_t));

    Dh::Printf(L"\n`%s`\n"
               L"FileCount  : %16d (%2.8f sec.)\n"
               L"DirSize    : %16s\n"
               L"UsedSize   : %16s\n"
               L"BufferSize : %16s\n\n"
               , path
               , count
               , tm.Seconds()
               , Str::HumanSize(dsize.QuadPart).c_str()
               , Str::HumanSize(usize).c_str()
               , Str::HumanSize(bsize).c_str()
               );

    return count;
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int show_command) 
{
    const size_t bs = 1024 * 1024 * 16;
    boost::scoped_array<unsigned char> buffer(new unsigned char[bs]);

    ReadDir(L"e:\\--\\projroot\\ffw\\", buffer.get(), bs, true);

  //ReadDir(L"M:\\master\\", buffer.get(), bs, true);

  //ReadDir(L"C:\\Windows\\System32", buffer.get(), bs, false);
  //ReadDir(L"C:\\Windows\\SysWOW64", buffer.get(), bs, false);
  //ReadDir(L"C:\\Windows\\WinSxS", buffer.get(), bs, false);
  //ReadDir(L"C:\\temp", buffer.get(), bs, false);
  //ReadDir(L"\\", buffer.get(), bs, false);
  //ReadDir(L"\\\\.\\C:\\", buffer.get(), bs, false);
    
    return 0;
}
