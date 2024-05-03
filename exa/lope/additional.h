#pragma once

#if 0
namespace
{
    static HRESULT _ParseFromPoint(CComPtr<IUIAutomation> const& automation, CPoint const& pt)
    {
        CComPtr<IUIAutomationElement> element;
        HRESULT hr = automation->ElementFromPoint(pt, &element);

        if (!element)
        {
            Dh::Printf(_T("[%04d %04d] failed `%s`\n"), pt.x, pt.y, Str::ErrorCode<wchar_t>::SystemMessage(hr));
            return hr;
        }

        CComPtr<ISelectionProvider> selection;
        element->GetCurrentPattern(UIA_SelectionPatternId, (IUnknown**)&selection);
        if (selection)
        {
            SAFEARRAY* arr = NULL;
            HRESULT hr = selection->GetSelection(&arr);
            if (SUCCEEDED(hr))
            {
                LONG n = 0;
                BSTR pv = NULL;
                hr = ::SafeArrayGetElement(arr, &n, &pv);
                if (SUCCEEDED(hr))
                {
                    Dh::Printf(_T("`%s`\n"), pv);
                }
            }
        }

        CONTROLTYPEID typeId = 0;
        HRESULT hrTypeId = element->get_CurrentControlType(&typeId);

        BSTR name;
        HRESULT hrName = element->get_CurrentName(&name);

        Dh::Printf(L"[%04d %04d] %06d %s `%s`\n", pt.x, pt.y
            , SUCCEEDED(hrTypeId) ? typeId : -1
            , _ControlTypeString(typeId)
            , SUCCEEDED(hrName) ? name : Str::ErrorCode<wchar_t>::SystemMessage(hrName)
            );

        return S_OK;
    }
}

#include <winsock2.h>
#include <windows.h>
#include <iostream>
#pragma comment(lib,"ws2_32.lib")

using namespace std;
int main (){
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        cout << "WSAStartup failed.\n";
        system("pause");
        return 1;
    }
    SOCKET Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    struct hostent *host;
    host = gethostbyname("www.google.com");
    SOCKADDR_IN SockAddr;
    SockAddr.sin_port=htons(80);
    SockAddr.sin_family=AF_INET;
    SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);
    cout << "Connecting...\n";
    if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr)) != 0){
        cout << "Could not connect";
        system("pause");
        return 1;
    }
    cout << "Connected.\n";
    send(Socket,"GET / HTTP/1.1\r\nHost: www.google.com\r\nConnection: close\r\n\r\n", strlen("GET / HTTP/1.1\r\nHost: www.google.com\r\nConnection: close\r\n\r\n"),0);
    char buffer[10000];
    int nDataLength;
    while ((nDataLength = recv(Socket,buffer,10000,0)) > 0){        
        int i = 0;
        while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') {
            cout << buffer[i];
            i += 1;
        }
    }
    closesocket(Socket);
    WSACleanup();
    system("pause");
    return 0;
}
#endif

