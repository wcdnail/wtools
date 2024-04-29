#pragma once

namespace CF
{
    struct DialogAttrs
    {
        enum : unsigned
        {
            /* Buttons */
            Ok                    = 0x00000001,
            Yes                   = 0x00000002,
            No                    = 0x00000004,
            Cancel                = 0x00000008,
            Abort                 = 0x00000010,
            Retry                 = 0x00000020,
            Ignore                = 0x00000040,
            Close                 = 0x00000080,
            Help                  = 0x00000100,
            Continue              = 0x00000200,

            /* Stock icons */
            IconStop              = 0x01000000,
            IconQuestion          = 0x02000000,
            IconExclamation       = 0x04000000,
            IconAsterix           = 0x08000000,
            IconCustom            = 0x10000000,

            /* Buttons */
            OkCancel              = Ok | Cancel,
            AbortRetryIgnore      = Abort | Retry | Ignore,
            YesNo                 = Yes | No,
            YesNoCancel           = Yes | No | Cancel,
            RetryCancel           = Retry | Cancel,
            CancelRetryContinue   = Cancel | Retry | Continue,

            /* Masks */
            HasButtons            = Ok | Cancel | Abort | Retry | Ignore | Yes | No | Close | Help,
            HasIcons              = IconStop | IconQuestion | IconExclamation | IconAsterix | IconCustom,
        };
    };

    struct DialogResult
    {
        enum : int
        {
            Ok         = 1,
            Yes        = 2,
            No         = 3,
            Cancel     = 4,
            Abort      = 5,
            Retry      = 6,
            Ignore     = 7,
            Close      = 8,
            Help       = 9,
            Continue   = 10,
        };

        int Code = Cancel;
    };
}
