#pragma once

#include <string>
#include <boost/system/error_code.hpp>

namespace Fl
{
    class Entry;
    class List;
}

namespace boost
{
    namespace filesystem
    {
        class path;
    }
}

namespace Twins
{
    class PromptDialog;
    
    namespace Sy = boost::system;
    namespace Fs = std::filesystem;

    typedef Sy::error_code OperationResult;
    typedef Fl::List const& FileList;
    typedef unsigned __int64 FileSize;

    struct SpecFlags
    {
        enum
        {
            None                        = 0,
            SaveNtfsAttributes          = 0x01000000, // "Сохранять атрибуты NTFS"
            SkipError                   = 0x02000000, // "Пропускать файлы, недоступные для чтения"
            ReplaceDeleteHiddenSystem   = 0x04000000, // "Переписать/удалить скрытые/системные"
            Cancel                      = 0x08000000,
            AskUser                     = 0x00000001, // "Запрос подтверждения"
            Replace                     = 0x00000002, // "Замена"
            ReplaceAll                  = 0x00000004,
            Skip                        = 0x00000008, // "Пропуск"
            SkipAll                     = 0x00000010,
            ReplaceOlder                = 0x00000020, // "Замена более старого"
            ReplaceAllOlder             = 0x00000040, // "Замена более старого"
            Rename                      = 0x00000080,
            AutoRenameSource            = 0x00000100, // "Авто-переименовать исходный файл"
            AutoRenameDest              = 0x00000200, // "Авто-переименовать целевой файл"
            Append                      = 0x00000400,
            ReplaceAllBiggest           = 0x00000800,
            ReplaceAllSmallest          = 0x00001000,
        };

        SpecFlags(unsigned initial = None): Value(initial) {}
        bool Check(unsigned mask) const { return 0 != (Value & mask); }
        void Add(unsigned mask) { Value |= mask; }

        bool IsCanceled() const { return Check(Cancel); }
        bool IsSkipped() const { return Check(Skip); }
        void CheckSilent();

        unsigned Value;
    };

    inline void SpecFlags::CheckSilent()
    {
        if (Check(AskUser) && 
            Check(ReplaceAll | SkipAll 
                | AutoRenameSource | AutoRenameDest 
                | ReplaceAllBiggest | ReplaceAllSmallest
                | ReplaceAllOlder
            ))
        {
            Value ^= AskUser;
        }
    }
}
