#include "stdafx.h"
#include "NetworkManager.h"
#include <XML.h>
#include <Lm.h>
#include <iads.h>
#include <adshlp.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <algorithm>

DWORD GetHosts(CString const& domainName, CStringVector& serverNames, bool sort)
{
   serverNames.clear();
	LPSERVER_INFO_100 info100 = NULL;
	DWORD readLen = 0;
	DWORD total = 0;
    NET_API_STATUS rv = ::NetServerEnum(0, 100, (LPBYTE*)&info100, MAX_PREFERRED_LENGTH, &readLen, &total, SV_TYPE_WORKSTATION | SV_TYPE_NT, domainName.GetString(), 0);

	if (((NERR_Success == rv) || (ERROR_MORE_DATA == rv)) && (NULL != info100))
	{
        rv = ERROR_SUCCESS;
		for (DWORD i=0; i<readLen; i++)
		{
         serverNames.push_back(info100[i].sv100_name);
		}

        if (sort)
        {
            std::sort(serverNames.begin(), serverNames.end(), std::less<CStringW>());
        }
	}

	if (NULL != info100)
    {
        ::NetApiBufferFree( info100 );
    }

	return rv;
}

DWORD GetDomains(CStringVector& domainNames)
{
    LPSERVER_INFO_100 info100 = NULL;
    DWORD count = 0, total = 0;

    NET_API_STATUS rv = ::NetServerEnum(0, 100, (LPBYTE*)&info100, MAX_PREFERRED_LENGTH, &count, &total, SV_TYPE_DOMAIN_ENUM, NULL, NULL);
    if (((NERR_Success == rv) || (ERROR_MORE_DATA == rv)) && (NULL != info100))
    {
        rv = ERROR_SUCCESS;
        for (DWORD i=0; i<count; i++)
        {
            domainNames.push_back(info100[i].sv100_name);
        }
    }

    if (NULL != info100)
    {
        ::NetApiBufferFree(info100);
    }

    return rv;
}

HRESULT EnumerateADObjects(CStringW const& domainName, CStringVector& serverNames)
{
   CStringW dottedName = ConvertDottedNameToDNName(domainName);

	CComPtr<IDirectorySearch> pISearch = NULL;
	CStringW sbstrBindingString = CStringW( L"LDAP://" ) + dottedName;

    HRESULT rv = ::ADsGetObject(sbstrBindingString, IID_IDirectorySearch, (void**)&pISearch);
	if (SUCCEEDED(rv))
	{
		LPWSTR pszAttr[] = { L"distinguishedname" };
		ADS_SEARCH_HANDLE hSearch;
		DWORD dwCount = 0x0;
		DWORD dwAttrNameSize = sizeof( pszAttr ) / sizeof( LPWSTR );

		rv = pISearch->ExecuteSearch(L"(objectCategory=computer)", pszAttr, dwAttrNameSize, &hSearch);
		ADS_SEARCH_COLUMN col;
		while (pISearch->GetNextRow(hSearch) != 0x00005012)
		{
			rv = pISearch->GetColumn(hSearch, L"distinguishedname", &col);
			if (SUCCEEDED(rv))
			{ 
				CStringW currentName = col.pADsValues->CaseIgnoreString;
				int index = currentName.Find(dottedName);
				if (index != -1)
					serverNames.push_back(ConvertDNNameToDottedName(currentName.Left(index))); 
				pISearch->FreeColumn( &col );
			}

			++dwCount;
			if (dwCount >= 10000)
                break;
		}
		pISearch->CloseSearchHandle(hSearch);
	}

	return rv;
}

CStringW ConvertDNNameToDottedName(CStringW const& source)
{
	CStringW result;

	if (source.IsEmpty()) 
        return result;

	int index = 0;
	CStringW objectName = source;
	while (index != -1)
	{
		CStringW itemName;
		index = objectName.ReverseFind(L',');
		if (index != -1)
		{
			itemName = objectName.Right(objectName.GetLength() - index - 1);
			objectName = objectName.Left(index);
		}
		else
			itemName = objectName;

		int eqIndex = itemName.Find(L'=');
		if (eqIndex != -1)
		{
			if ( result.IsEmpty() )
				result = itemName.Right(itemName.GetLength() - eqIndex - 1);
			else
				result = itemName.Right(itemName.GetLength() - eqIndex - 1) + CStringW(L".") + result;
		}
	}

	return result;
}

CStringW ConvertDottedNameToDNName(CStringW const& source)
{
	CStringW result;

	if (source.IsEmpty())
        return result;

	int index = 0;
	CStringW objectName = source;
	while ( index != -1 )
	{
		CStringW itemName;
		index = objectName.ReverseFind(L'.');
		if ( index != -1 )
		{
			itemName = objectName.Right(objectName.GetLength() - index - 1);
			objectName = objectName.Left(index);
		}
		else
			itemName = objectName;

		if ( result.IsEmpty() )
			result = CStringW(L"DC=") + itemName;
		else
			result = CStringW(L"DC=") + itemName + CStringW(L",") + result;
	}

	return result;
}

bool PingHost(unsigned long ip, ULONG* statusPtr)
{
    bool rv = false;

    unsigned long ipaddr = ::htonl(ip);
	if (ipaddr == INADDR_NONE)
    {
        return rv;
    }

	HANDLE icmp = ::IcmpCreateFile();
	if (icmp == INVALID_HANDLE_VALUE)
    {
        return rv;
    }

    char sendData[] = "1234567890";
    BYTE buffer[sizeof(ICMP_ECHO_REPLY) + sizeof(sendData)];
    DWORD replySize = _countof(buffer);
    LPVOID bufferPtr = buffer;
    DWORD dwRetVal = ::IcmpSendEcho(icmp, ipaddr, sendData, sizeof(sendData), NULL, bufferPtr, replySize, 100);
	PICMP_ECHO_REPLY echoReply = (PICMP_ECHO_REPLY)bufferPtr;
	rv = (echoReply->Status == IP_SUCCESS);

    if (NULL != statusPtr)
    {
        *statusPtr = echoReply->Status;
    }

	::IcmpCloseHandle(icmp);
	return rv;

}

HRESULT GetAdDomains(CStringW& rootDomain, CStringVector& domains, bool sort)
{
	IADsContainer* pIADsCont = NULL;
	CComBSTR sbstrBindingString = L"GC:";
    HRESULT rv = ::ADsGetObject(sbstrBindingString, IID_IADsContainer, (void**)&pIADsCont);
    if (SUCCEEDED(rv))
	{
		IEnumVARIANT *pEnumVariant = NULL;
        rv = ::ADsBuildEnumerator(pIADsCont, &pEnumVariant);
        if (SUCCEEDED(rv))
		{
			while (true)
			{
                VARIANT Variant = {0};
                ULONG ulElementsFetched = 0;
                HRESULT hr = ::ADsEnumerateNext(pEnumVariant, 1, &Variant, &ulElementsFetched);
                if (FAILED(hr))
                    break;
                else if (ulElementsFetched < 1)
                    break;

				if ( VT_DISPATCH == Variant.vt )
				{
					IADs* pIADs= NULL;
					if (SUCCEEDED( Variant.pdispVal->QueryInterface(IID_IADs, (VOID**)&pIADs)))
					{
						// Print the object data.
						CComBSTR sbstrResult, strClass, path;
						hr = pIADs->get_Name( &sbstrResult );
						hr = pIADs->get_Class( &strClass );
						hr = pIADs->get_ADsPath( &path );
						CStringW pathString = path;
						if (SUCCEEDED(hr) && strClass == CComBSTR( L"top" ))
							rootDomain = pathString.Right( pathString.GetLength() - 0x5 );
						pIADs->Release();
					}
				}
                ::VariantClear(&Variant);
			}
			pEnumVariant->Release();
		}
		pIADsCont->Release();
	}

	if (rootDomain.IsEmpty())
		return S_OK;

    // TODO: refactor THIS shit!

	sbstrBindingString = CComBSTR() + "GC://" + NetworkManager::ConvertDottedNameToDNName( rootDomain );
    HRESULT rv2 = ::ADsGetObject(sbstrBindingString, IID_IADsContainer, (void**) &pIADsCont);
    if ( SUCCEEDED(rv2) )
	{
		IEnumVARIANT *pEnumVariant = NULL;
		if ( SUCCEEDED( ADsBuildEnumerator(pIADsCont, &pEnumVariant) ) )
		{
			VARIANT Variant;
			ULONG ulElementsFetched;
			while ( SUCCEEDED( ADsEnumerateNext( pEnumVariant, 0x1, &Variant, &ulElementsFetched ) ) && ( ulElementsFetched > 0 ) )
			{
				if ( VT_DISPATCH == Variant.vt )
				{
					IADs* pIADs= NULL;
					if ( SUCCEEDED( Variant.pdispVal->QueryInterface(IID_IADs, (VOID**)&pIADs) ) )
					{
						// Print the object data.
						CComBSTR sbstrResult, strClass, path;
						HRESULT hr = pIADs->get_Name( &sbstrResult );
						hr = pIADs->get_Class( &strClass );
						hr = pIADs->get_ADsPath( &path );
						CStringW pathString = path;
						if ( SUCCEEDED( hr ) && strClass == CComBSTR( L"domainDNS" ) )
							domains.push_back( NetworkManager::ConvertDNNameToDottedName( pathString.Right( pathString.GetLength() - 0x7 ) ) );
						pIADs->Release();
					}
				}
				VariantClear(&Variant);
			}
			pEnumVariant->Release();
		}
		pIADsCont->Release();
	}

    if (sort)
    {
        std::sort(domains.begin(), domains.end(), std::less<CStringW>());
    }

    return rv;
}

void GetAdDomainsHosts(CStringVector& hosts, bool sort)
{
	CString rootDomain;
	CStringVector domains;
	GetAdDomains(rootDomain, domains, sort);
	domains.push_back(rootDomain);
	CStringVector adObjects;

	for(CStringVector::const_iterator c_it = domains.begin(); c_it != domains.end(); ++c_it)
	{
		EnumerateADObjects(ConvertDottedNameToDNName(*c_it), adObjects);
	}

	int dotIdx;
	for(CStringVector::const_iterator c_it = adObjects.begin(); c_it != adObjects.end(); ++c_it)
	{
		dotIdx = c_it->Find(L".");
		if(dotIdx != -1)
			hosts.push_back(c_it->Left(dotIdx));
		else
			hosts.push_back(*c_it);
	}
}

DWORD GetHostIp(PCWSTR hostName, PWSTR destIpStr, int destIpStrSize)
{
    ADDRINFOW addrHint = {0};
    addrHint.ai_family = AF_UNSPEC;
    addrHint.ai_socktype = SOCK_STREAM;
    addrHint.ai_protocol = IPPROTO_TCP;

    ADDRINFOW* pAddrInfo = NULL;
    DWORD rv = ::GetAddrInfoW(hostName, NULL, &addrHint, &pAddrInfo);
    if (ERROR_SUCCESS == rv)
    {
        for (ADDRINFOW* pAi=pAddrInfo; pAi!=NULL; pAi=pAi->ai_next)
        {
            if (AF_INET == pAi->ai_family)
            {
                PSOCKADDR pSockAddr = (PSOCKADDR)pAi->ai_addr;
                DWORD buffLen = destIpStrSize;
                rv = ::WSAAddressToStringW(pSockAddr, (DWORD)pAi->ai_addrlen, NULL, destIpStr, &buffLen);
                if (ERROR_SUCCESS == rv)
                    break;
            }
        }

        ::FreeAddrInfoW(pAddrInfo);
    }	

    return rv;
}

CString GetHostName(unsigned long ip)
{
   sockaddr_in sockAddr;
   sockAddr.sin_family = AF_INET;
   sockAddr.sin_addr.S_un.S_addr = ip;

   char hostNameA[512] = {0};
   ::getnameinfo((sockaddr*)&sockAddr, sizeof(sockaddr_in), hostNameA, _countof(hostNameA), NULL, 0, NI_NOFQDN);

   return CString(CA2W(hostNameA));
}
