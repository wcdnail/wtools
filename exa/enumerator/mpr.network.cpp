#include "stdafx.h"
#include "mpr.network.h"
#include <string>

namespace Mpr
{
    PCWSTR DispTypeString(DWORD type)
    {
        switch (type) 
        {
        case RESOURCEDISPLAYTYPE_GENERIC: return L"generic";
        case RESOURCEDISPLAYTYPE_DOMAIN:  return L"domain";
        case RESOURCEDISPLAYTYPE_SERVER:  return L"server";
        case RESOURCEDISPLAYTYPE_SHARE:   return L"share";
        case RESOURCEDISPLAYTYPE_FILE:    return L"file";
        case RESOURCEDISPLAYTYPE_GROUP:   return L"group";
        case RESOURCEDISPLAYTYPE_NETWORK: return L"network";
        }

        return L"unknown";
    }

    static void AppendTo(std::wstring& result, wchar_t const* x)
    {
        if (!result.empty())
            result += L' ';

        result += x;
    }

    std::wstring UsageString(DWORD usage)
    {
        std::wstring result;

        if (0 != (usage & RESOURCEUSAGE_CONNECTABLE))   
            AppendTo(result, L"connectable");

        if (0 != (usage & RESOURCEUSAGE_CONTAINER))     
            AppendTo(result, L"container");

        if (0 != (usage & RESOURCEUSAGE_NOLOCALDEVICE)) 
            AppendTo(result, L"nolocaldevice");

        if (0 != (usage & RESOURCEUSAGE_SIBLING))       
            AppendTo(result, L"sibling");

        if (0 != (usage & RESOURCEUSAGE_ATTACHED))      
            AppendTo(result, L"attached");

        if (0 != (usage & RESOURCEUSAGE_RESERVED))      
            AppendTo(result, L"reserved");

        return result;
    }

    bool Enumerate(LPNETRESOURCEW lpnr, DWORD scope, DWORD type, DWORD usage, int deep, bool shallAll)
    {
        HANDLE tempEnum = NULL;
        DWORD rc = ::WNetOpenEnumW(scope, type, usage, lpnr, &tempEnum);
        if (rc != NO_ERROR) 
            return false;

        EnumHandle hEnum(tempEnum, WNetCloseEnum);

        do 
        {
            BYTE buffer[65536] = {0};
            DWORD bufferSize = sizeof(buffer);
            DWORD count = -1;
            LPNETRESOURCEW nres = (LPNETRESOURCEW)buffer;

            rc = ::WNetEnumResourceW(hEnum, &count, nres, &bufferSize);
            if (ERROR_MORE_DATA == rc)
            {
                // TODO: handle this
            }

            if (NO_ERROR == rc) 
            {
                for (DWORD i=0; i<count; i++) 
                {
                    for (int d=0; d<deep; d++)
                        ::wprintf(L"  ");

                    ::wprintf(L"%s [%s [%s]]\n"
                        , nres[i].lpRemoteName
                        , DispTypeString(nres[i].dwDisplayType)
                        , UsageString(nres[i].dwUsage).c_str()
                        );

                    bool deeper = shallAll ? true : (RESOURCEDISPLAYTYPE_SERVER != nres[i].dwDisplayType);
                    if (deeper && (0 != (nres[i].dwUsage & RESOURCEUSAGE_CONTAINER)))
                    {
                        Enumerate(&nres[i], scope, type, usage, deep + 1, shallAll);
                    }
                }
            }
        }
        while (rc != ERROR_NO_MORE_ITEMS);

        return true;
    }
}
