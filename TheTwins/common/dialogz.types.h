#pragma once

namespace Cf
{
    struct DialogResult
    {
        static const int Ok         = 1;
        static const int Yes        = 2;
        static const int No         = 3;
        static const int Cancel     = 4;
        static const int Abort      = 5;
        static const int Retry      = 6;
        static const int Ignore     = 7;
        static const int Close      = 8;
        static const int Help       = 9;
        static const int Continue   = 10;

        DialogResult() 
            : rv(Cancel)
        {}

        operator int () const { return rv; }

        void Set(int x)
        {
            if ((x >= Ok) && (x <=Help))
                rv = x;
        }

    private:
        int rv;
    };

    struct DialogFlags
    {
        enum
        {
        /* Buttons */
          Ok                    = 0x00000001
        , Yes                   = 0x00000002
        , No                    = 0x00000004
        , Cancel                = 0x00000008
        , Abort                 = 0x00000010
        , Retry                 = 0x00000020
        , Ignore                = 0x00000040
        , Close                 = 0x00000080
        , Help                  = 0x00000100
        , Continue              = 0x00000200

        /* Stock icons */
        , IconStop              = 0x01000000
        , IconQuestion          = 0x02000000
        , IconExclamation       = 0x04000000
        , IconAsterix           = 0x08000000

        /* Buttons */
        , OkCancel              = Ok | Cancel
        , AbortRetryIgnore      = Abort | Retry | Ignore
        , YesNo                 = Yes | No
        , YesNoCancel           = Yes | No | Cancel
        , RetryCancel           = Retry | Cancel
        , CancelRetryContinue   = Cancel | Retry | Continue

        /* Masks */
        , HasButtons            = Ok | Cancel | Abort | Retry | Ignore | Yes | No | Close | Help
        , HasIcons              = IconStop | IconQuestion | IconExclamation | IconAsterix
        };
    };
}
