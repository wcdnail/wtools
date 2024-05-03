#include "stdafx.h"
#include "lm.network.h"

namespace Net
{
    void DumpInfo(SERVER_INFO_101 const& info101)
    {
        ::wprintf(L"%s [%x%s%s %d %d.%d] %s\n", info101.sv101_name
            , info101.sv101_type
            , (0 != (info101.sv101_type & SV_TYPE_DOMAIN_CTRL)) ? L" pdc" : L""
            , (0 != (info101.sv101_type & SV_TYPE_DOMAIN_BAKCTRL)) ? L" bdc" : L""
            , info101.sv101_platform_id
            , info101.sv101_version_major, info101.sv101_version_minor
            , info101.sv101_comment
            );
    }

    NET_API_STATUS EnumerateHosts(PCWSTR domainName)
    {
        DWORD resume = 0;
        DWORD total = 0;
        DWORD count = 0;
        LPSERVER_INFO_101 info101 = NULL;
        NET_API_STATUS rc = ::NetServerEnum(NULL, 101, (LPBYTE *)&info101, MAX_PREFERRED_LENGTH, &count, &total, SV_TYPE_SERVER, domainName, &resume);
        if (((rc == NERR_Success) || (rc == ERROR_MORE_DATA)) && (NULL != info101)) 
        {
            LPSERVER_INFO_101 current = info101;
            for (DWORD i=0; i<count; i++) 
            {
                if (current != NULL)
                {
                    ::wprintf(L"  ");
                    DumpInfo(*current);
                }

                ++current;
            }

            //if (rc == ERROR_MORE_DATA) 
            //{
            //}
        } 

        if (info101 != NULL)
            ::NetApiBufferFree(info101);

        return rc;
    }

    NET_API_STATUS EnumerateDomains()
    {
        DWORD total = 0;
        DWORD count = 0;
        LPSERVER_INFO_101 info101 = NULL;
        NET_API_STATUS rc = ::NetServerEnum(0, 101, (LPBYTE*)&info101, MAX_PREFERRED_LENGTH, &count, &total, SV_TYPE_DOMAIN_ENUM, NULL, NULL);
        if (((NERR_Success == rc) || (ERROR_MORE_DATA == rc)) && (NULL != info101))
        {
            for (DWORD i=0; i<count; i++)
            {
                DumpInfo(info101[i]);
                EnumerateHosts(info101[i].sv101_name);
            }
        }

        if (NULL != info101)
            ::NetApiBufferFree(info101);

        return rc;
    }
}
