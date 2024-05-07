#include "stdafx.h"
#include "file.view.buffer.h"
#include <dh.tracing.h>
#include <string.utils.error.code.h>
#include <string.utils.human.size.h>

namespace Fv
{
    FileBuffer::~FileBuffer()
    {
        if (!Filepath.empty())
        {
            Dh::TPrintf(L"FvBuffer: dtor `%s` %I64u - %I64u x %I64u\n"
                , Filepath.c_str()
                , FileSize, BlockCount, BlockSize
                );
        }
    }

    FileBuffer::FileBuffer()
        : Filepath()
        , Input() 
        , FileSize(0)
        , BlockSize(0)
        , BlockCount(0)
        , Blocks()
    {}

    FileBuffer::ByteArray const& FileBuffer::GetBlock(SizeType index) const
    {
        BlockStore::const_iterator it = Blocks.find(index);
        if (it == Blocks.end())
        {
            ByteArray bytes(new Byte[BlockSize]);
            ::memset(bytes.get(), 0, BlockSize);

            DataBlock def;
            def.bytes = bytes;
            def.size = 0;

            ReadBlock(index, def);
            it = Blocks.insert(std::make_pair(index, def)).first;
        }

        return it->second.bytes;
    }

    ErrorCode FileBuffer::ReadBlock(SizeType index, DataBlock& def) const
    {
        ErrorCode error;
        HRESULT hr;

        LARGE_INTEGER lrOffset = {};
        lrOffset.QuadPart = index * BlockSize;

        if (!::SetFilePointerEx(Input.get(), lrOffset, NULL, FILE_BEGIN))
        {
            hr = ::GetLastError();
            error.assign((int)hr, boost::system::system_category());
        }
        else
        {
            DWORD readed = 0;
            if (!::ReadFile(Input.get(), def.bytes.get(), (DWORD)BlockSize, &readed, NULL))
            {
                hr = ::GetLastError();
                error.assign((int)hr, boost::system::system_category());
            }
            def.size = readed;
        }

        Dh::TPrintf(L"FvBuffer: read >%I64u #%I64u = %I64u (%S(%d))\n"
            , lrOffset.QuadPart, index, def.size
            , error.message().c_str(), error.value()
            );

        return error;
    }

    Byte FileBuffer::GetByte(SizeType offset) const 
    {
        if (!OffsetInRange(offset) || (0 == BlockSize))
            return 0;

        SizeType blockIndex = offset / BlockSize;
        SizeType offsetInBlock = offset - (blockIndex * BlockSize);

        return GetBlock(blockIndex)[offsetInBlock]; 
    }

    bool FileBuffer::OffsetInRange(SizeType offset) const { return (offset >= 0) && (offset < FileSize); }
    bool FileBuffer::Ok() const { return NULL != Input.get(); }
    SizeType FileBuffer::GetFileSize() const { return FileSize; }

    ErrorCode FileBuffer::Reset(AutoHandle& input, SizeType blockSize)
    {
        LARGE_INTEGER liFileSize = {};
        if (!::GetFileSizeEx(input.get(), &liFileSize))
        {
            HRESULT hr = ::GetLastError();
            return ErrorCode((int)hr, boost::system::system_category());
        }

        SizeType fileSize = static_cast<SizeType>(liFileSize.QuadPart);
        SizeType blockCount = fileSize / blockSize + 1;

        FileSize = fileSize;
        BlockSize = blockSize;
        BlockCount = blockCount;
        Input.swap(input);
        Blocks.clear();

        return ErrorCode();
    }

    ErrorCode FileBuffer::Open(Path const& path, SizeType blockSize /*= 65536*/)
    {
        HRESULT hr;
        ErrorCode error;

        if (!blockSize || blockSize > (2lu * (1024lu * 1024lu * 1024lu)))
            error.assign(EINVAL, boost::system::generic_category());

        else
        {
            SYSTEM_INFO si = {};
            ::GetSystemInfo(&si);
            if (0 != (blockSize % si.dwAllocationGranularity))
                Dh::TPrintf(L"FvBuffer: open `%s` Allocation Granularity Warning `0 != %I64u %% %u`\n"
                , path.c_str(), blockSize, si.dwAllocationGranularity);

            HANDLE inp = ::CreateFileW(path.c_str(), GENERIC_READ
                , FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
                , NULL, OPEN_EXISTING, 0, NULL);
            hr = ::GetLastError();

            if (INVALID_HANDLE_VALUE == inp)
                error.assign((int)hr, boost::system::system_category());

            else
            {
                AutoHandle input(inp, CloseHandle);
                error = Reset(input, blockSize);
                if (!error)
                    Filepath = path;
            }
        }

        if (error)
            Dh::TPrintf(L"FvBuffer: open `%s` - %d - %S\n", path.c_str()
                , error.value(), error.message().c_str());
        else
            Dh::TPrintf(L"FvBuffer: open `%s` %I64u - %I64u x %I64u - OK\n", path.c_str()
                , FileSize, BlockCount, BlockSize);

        return error;
    }

    void FileBuffer::GetStatus(CStringW& text) const
    {
        if (!Filepath.empty())
        {
            text.Format(L"`%s @%s`"
                , Filepath.filename().c_str()
                , Str::HumanSize(FileSize)
                );
        }
    }

    bool FileBuffer::IsEol(SizeType offset) const { return '\n' == GetByte(offset); }
    static bool IsWordBreak(Byte sym) { return sym <= 32; }

    bool FileBuffer::IsLineLimit(SizeType begin, SizeType offset, SizeType maxlen) const
    {
        if (npos == maxlen)
            return false;

        return (offset - begin) > maxlen ? IsWordBreak(GetByte(offset)) : false;
    }

    SizeType FileBuffer::GetLineEnd(SizeType offset, SizeType maxlen) const
    {
        SizeType begin = offset;
        while (OffsetInRange(offset) && !IsLineLimit(begin, offset, maxlen) && !IsEol(offset++)) ;
        return offset;
    }

    static Byte GetPrintable(Byte sym, bool replaceSpecBySpace)
    {
        if (sym < 32)
        {
            if (replaceSpecBySpace)
                return ' ';

            static const Byte para = 0xb6;   // �
            static const Byte turtle = 0xa4; // �
            static const Byte retrn = 0xac;  // �
            static const Byte mpoint = 0xb7; // �

            Byte replacer = mpoint;

            switch (sym)
            {
            case '\n': replacer = retrn; break;
            case '\t': replacer = mpoint; break;
            case '\a': replacer = turtle; break;
            }

            return replacer;
        }

        return sym;
    }

    CStringW FileBuffer::GetTextLine(SizeType beg, SizeType end, unsigned cp, bool showspec) const
    {
        CStringW rv;

        if ((end > beg) && (end <= FileSize))
        {
            SizeType size = end - beg;
            Byte* temp = (Byte*)::calloc(size + 1, 1);
            if (NULL != temp)
            {
                for (SizeType i=0; i<size; i++)
                    temp[i] = GetPrintable(GetByte(beg + i), !showspec);

                rv = CA2W((PCSTR)temp, cp).m_psz;
                ::free(temp);
            }
        }

        return rv;
    }

    SizeType FileBuffer::FindNextLine(int count, SizeType offset, SizeType maxlen) const
    {
        while (OffsetInRange(offset) && count--)
            offset = GetLineEnd(offset, maxlen);

        return offset;
    }

    SizeType FileBuffer::GetLineStart(SizeType offset) const
    {
        while ((offset > 0) && !IsEol(offset--)) ;

        while ((offset > 0) && !IsEol(offset)) 
            --offset;

        return 0 == offset ? 0 : ++offset;
    }

    SizeType FileBuffer::FindPrevLine(int count, SizeType offset) const
    {
        if (!OffsetInRange(offset))
            offset = FileSize - 1;

        while ((offset > 0) && OffsetInRange(offset) && count--)
            offset = GetLineStart(offset);

        return offset;
    }

    SizeType FileBuffer::FindPrevLineFromEnd(int count) const
    {
        return FindPrevLine(count, FileSize);
    }

    SizeType FileBuffer::GetFixedLineEnd(SizeType offset, int count) const
    {
        SizeType rv = offset + count;
        return rv > FileSize ? FileSize : rv;
    }

    void FileBuffer::CheckFileLimit(SizeType& offset, SizeType displayCount) const
    {
        SizeType limit = displayCount < FileSize ? FileSize - displayCount : 0;
        offset = offset > limit ? limit : offset;
    }

    CStringW FileBuffer::GetHexLine(SizeType beg, SizeType end) const
    {
        CStringW rv;        

        if ((end > beg) && (end <= FileSize))
        {
            SizeType size = end - beg;

            for (SizeType i=0; i<size; i++)
                rv.AppendFormat(L"%02x ", GetByte(beg + i));
        }

        return rv;
    }
}
