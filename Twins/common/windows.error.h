#pragma once

#if 0                   // std::error_code
#include <atlstr.h>
#include <winerror.h>

namespace Cf // Common framework
{
    class ErrorDesc
    {
    public:
        virtual ~ErrorDesc();
        virtual void SetError(PCTSTR addText = NULL, DWORD code = GetLastError());

        ATL::CString const& GetErrorText() const { return text_; }
        HRESULT GetErrorCode() const { return code_; }

    protected:
        ErrorDesc();

    private:
        HRESULT code_;
        ATL::CString text_;

    private:
        ErrorDesc(ErrorDesc const&);
        ErrorDesc& operator = (ErrorDesc const&);
    };

    inline ErrorDesc::~ErrorDesc()
    {
    }

    inline ErrorDesc::ErrorDesc()
        : code_(S_OK)
        , text_(_T("Unknown error"))
    {
    }

    inline void ErrorDesc::SetError(PCTSTR addText/* = NULL*/, DWORD code /*= GetLastError()*/)
    {
        ATL::CString tempText = addText;

        ATL::CString tempCode;
        tempCode.Format(_T(" (%d)"), code);

        HLOCAL tempMem = NULL;
        DWORD l = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
            , NULL, (DWORD)code, 0, (PWSTR)&tempMem, 0, NULL);
        if (l > 2)
        {
            PTSTR message = (PTSTR)LocalLock(tempMem);
            message[l-2] = _T('\0');
            tempText += message;
            ::LocalUnlock(tempMem);
        }
        ::LocalFree(tempMem);

        code_ = code;
        tempText += tempCode;
        text_ = tempText;
    }
}

#endif
