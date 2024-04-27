#include "stdafx.h"
#include "shared.memory.h"
#include <string.utils.error.code.h>
#include <atlstr.h>

namespace Shelltastix
{
    namespace Shmem
    {
        Exception::Exception(HRESULT code, PCWSTR text, ...)
            : message_()
            , message2_()
            , code_(code)
        {
            try
            {
                va_list ap;
                va_start(ap, text);
                CStringW format;
                format.FormatV(text, ap);
                va_end(ap);

                message_ = (PCWSTR)format;
            }
            catch (...)
            {}
        }

        Exception::~Exception() 
        {}

        char const* Exception::what() const
        {
            char temp[128] = {0};

            try
            {
                message2_  = CW2A(message_.c_str()).m_psz;
                message2_ += "\r\n";
                message2_ += ::_itoa(code_, temp, 10);
                message2_ += " - `";
                message2_ += Str::ErrorCode<char>::SystemMessage(code_);
                message2_ += "`";
            }
            catch (...)
            {}

            return message2_.c_str();
        }

        HRESULT Exception::hresult() const
        {
            return code_;
        }

        AutoLock::~AutoLock() 
        {}

        HRESULT InitSecurity(String const& user, 
            SecurityAttributesPtr& sa, 
            HandlePtr& sid, 
            AclPtr& acl, 
            HandlePtr& desc)
        {
            if (!user.empty())
            {
                EXPLICIT_ACCESSW ea[2] = {0};

                // initialize an EXPLICIT_ACCESS structure for an ACE
                // the ACE will allow Everyone full access
                ea[0].grfAccessPermissions	= GENERIC_ALL;
                ea[0].grfAccessMode			= SET_ACCESS;
                ea[0].grfInheritance		= NO_INHERITANCE;
                ea[0].Trustee.TrusteeForm	= TRUSTEE_IS_NAME;
                ea[0].Trustee.TrusteeType	= TRUSTEE_IS_USER;
                ea[0].Trustee.ptstrName		= (PWSTR)user.c_str();

                // create a SID for the BUILTIN\Administrators group
                SID_IDENTIFIER_AUTHORITY authId = SECURITY_CREATOR_SID_AUTHORITY;
                PSID rawSid = NULL;
                if (!::AllocateAndInitializeSid(&authId, 1, SECURITY_CREATOR_OWNER_RID, 0, 0, 0, 0, 0, 0, 0, &rawSid)) 
                    return ::GetLastError();

                HandlePtr tsid(rawSid, ::FreeSid);

                // initialize an EXPLICIT_ACCESS structure for an ACE
                // the ACE will allow the Administrators group full access
                ea[1].grfAccessPermissions	= GENERIC_ALL;
                ea[1].grfAccessMode			= SET_ACCESS;
                ea[1].grfInheritance		= NO_INHERITANCE;
                ea[1].Trustee.TrusteeForm	= TRUSTEE_IS_SID;
                ea[1].Trustee.TrusteeType	= TRUSTEE_IS_WELL_KNOWN_GROUP;
                ea[1].Trustee.ptstrName		= (PWSTR)tsid.get();

                PACL rawAcl = NULL;
                DWORD code = ::SetEntriesInAclW(2, ea, NULL, &rawAcl);
                if (ERROR_SUCCESS != code)
                    return code;

                AclPtr tacl(rawAcl, ::LocalFree);
                HandlePtr tdesc(::LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH), ::LocalFree);
                if (!tdesc) 
                    return ::GetLastError();

                if (!::InitializeSecurityDescriptor(tdesc.get(), SECURITY_DESCRIPTOR_REVISION)) 
                    return ::GetLastError();

                // add the ACL to the security descriptor
                if (!::SetSecurityDescriptorDacl(tdesc.get(), TRUE, tacl.get(), FALSE))
                    return ::GetLastError();

                // initialize a security attributes structure
                SecurityAttributesPtr tsa(new SECURITY_ATTRIBUTES);
                tsa->nLength = sizeof (SECURITY_ATTRIBUTES);
                tsa->lpSecurityDescriptor = tdesc.get();
                tsa->bInheritHandle = FALSE;

                sa = tsa;
                sid = tsid;
                acl = tacl;
                desc = tdesc;
            }

            return S_OK;
        }

        void CreateSyncObjects(SecurityAttributesPtr const& sa, int flags, String const& name,
            HandlePtr& mx, HandlePtr& evReq, HandlePtr& evRsp)
        {
            HandlePtr tmx;
            HandlePtr tevReq;
            HandlePtr tevRsp;

            if (flags >= SyncObjectsRequest)
            {
                String mxName = name + L"_Mx";
                String evName = name + L"_EvReq";

                tmx = HandlePtr(
                    ::CreateMutex(sa.get(), FALSE, mxName.c_str()),
                    ::CloseHandle);

                if (!tmx)
                {
                    HRESULT hr = ::GetLastError();
                    throw Exception(hr, L"CreateMutex(`%s`) failed", mxName.c_str());
                }

                tevReq = HandlePtr(
                    ::CreateEvent(sa.get(), FALSE, FALSE, evName.c_str()),
                    ::CloseHandle);

                if (!tevReq)
                {
                    HRESULT hr = ::GetLastError();
                    throw Exception(hr, L"CreateEvent(`%s`) failed", evName.c_str());
                }
            }

            if (flags >= SyncObjectsBoth)
            {
                String evName = name + L"_EvResp";

                tevRsp = HandlePtr(
                    ::CreateEvent(sa.get(), FALSE, FALSE, evName.c_str()),
                    ::CloseHandle);

                if (!tevRsp)
                {
                    HRESULT hr = ::GetLastError();
                    throw Exception(hr, L"CreateEvent(`%s`) failed", evName.c_str());
                }
            }

            mx = tmx;
            evReq = tevReq;
            evRsp = tevRsp;
        }
    }
}
