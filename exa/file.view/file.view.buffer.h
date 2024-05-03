#pragma once

#include "types.h"
#include <map>
#include <atlstr.h>
#include <boost/shared_array.hpp>

namespace Fv
{
    class FileBuffer: boost::noncopyable
    {
    public:
        static const SizeType npos = 0xffffffffffffffff;

        FileBuffer();
        ~FileBuffer();

        bool Ok() const;
        ErrorCode Open(Path const& path, SizeType bs = 65536);
        void GetStatus(CStringW& text) const;
        SizeType GetFileSize() const;
        SizeType GetLineEnd(SizeType offset, SizeType maxlen) const;
        CStringW GetTextLine(SizeType beg, SizeType end, unsigned cp, bool showspec) const;
        SizeType FindNextLine(int count, SizeType offset, SizeType maxlen) const;
        SizeType FindPrevLine(int count, SizeType offset) const;
        SizeType FindPrevLineFromEnd(int count) const;
        SizeType GetFixedLineEnd(SizeType offset, int count) const;
        void CheckFileLimit(SizeType& offset, SizeType displayCount) const;
        CStringW GetHexLine(SizeType beg, SizeType end) const;

    private:
        typedef boost::shared_array<Byte> ByteArray;

        struct DataBlock
        {
            ByteArray bytes;
            SizeType size;
        };

        typedef std::map<SizeType, DataBlock> BlockStore;

        Path Filepath;
        AutoHandle Input;
        SizeType FileSize;
        SizeType BlockSize;
        SizeType BlockCount;
        mutable BlockStore Blocks;

        ErrorCode Reset(AutoHandle& input, SizeType bs);
        bool OffsetInRange(SizeType offset) const;
        Byte GetByte(SizeType offset) const;
        ByteArray const& GetBlock(SizeType index) const;
        ErrorCode ReadBlock(SizeType index, DataBlock& def) const;
        bool IsEol(SizeType offset) const;
        bool IsLineLimit(SizeType begin, SizeType offset, SizeType maxlen) const;
        SizeType GetLineStart(SizeType offset) const;
    };
}
