#include "stdafx.h"
#if 0
#include "crash.smtp.h"
#include "debug.output.h"
#include <malloc.h>

#ifdef _WIN32
#  include <winsock2.h>
#  include <assert.h>
#  include <strsafe.h>
#  ifdef _MSC_VER
#    pragma warning(push)
#    pragma warning(disable:4786)
#    pragma warning(disable:4996)
#    pragma comment(lib, "ws2_32")
#  endif
#endif

SOCKET ssmtp_connect(psimple_smtp sm);
int ssmtp_get_code(char const* buffer);
int ssmtp_recv(psimple_smtp sm, char* buffer, int len, unsigned timeout);
int ssmtp_send(psimple_smtp sm, char const* buffer, int len);

enum ssmtp_int_consts
{
    SsmtpBufferSize = 4096,
    SsmtpDelayMs = 10,
    SsmtpMessageSizeInMegs = 5,
    WsaNotInitialized = 0,
    WsaInitialized = 57
};

#define _free_mem(X)    \
    if (X)              \
    {                   \
        free((void*)X); \
        X = NULL;       \
    }

#define _close_sock(X)          \
    if (INVALID_SOCKET != X)    \
    {                           \
        closesocket(X);         \
        X = INVALID_SOCKET;     \
    }

#define _shutdown_sock(X, F)    \
    if (INVALID_SOCKET != X)    \
    {                           \
        shutdown(X, F);         \
        closesocket(X);         \
        X = INVALID_SOCKET;     \
    }

int simple_smtp_init(psimple_smtp sm, unsigned flags)
{
    if (!sm)
        return ErrorInvalidArgs;

    sm->errc = CSMTP_NO_ERROR;
    sm->server = NULL;
    sm->port = 0;
    sm->m_pcLogin = NULL;
    sm->m_pcPassword = NULL;
    sm->m_pcMailFrom = NULL;
    sm->m_pcNameFrom = NULL;
    sm->m_pcSubject = NULL;
    sm->m_pcMsgBody = NULL;
    sm->m_pcXMailer = NULL;
    sm->m_pcReplyTo = NULL;
    sm->m_iXPriority = SsmtpPriorityNormal;	

    sm->hSocket = INVALID_SOCKET;
    sm->wsaStartup = WsaNotInitialized;
    memset(&sm->wsaData, 0, sizeof(sm->wsaData));

    if (0 != (SsmtpNeedWsaStartup & flags))
    {
        WORD wsaVer = MAKEWORD(2, 2);

        if (WSAStartup(wsaVer, &sm->wsaData) != NO_ERROR)
            return sm->errc = WSAGetLastError();

        if ((2 != LOBYTE(sm->wsaData.wVersion)) || (2 != HIBYTE(sm->wsaData.wVersion))) 
        {
            WSACleanup();
            return sm->errc = CSMTP_WSA_VER;
        }

        sm->wsaStartup = WsaInitialized;
    }

    return 0;
}

void simple_smtp_delete(psimple_smtp sm)
{
    if (!sm)
        return ;

    _shutdown_sock(sm->hSocket, SD_BOTH);

    if (WsaInitialized == sm->wsaStartup)
        WSACleanup();
}

int simple_smtp_send(psimple_smtp sm)
{
    char sbuffer[SsmtpBufferSize] = {0};
    char rbuffer[SsmtpBufferSize] = {0};
    char localhost[256] = {0};
    int sblen = 0;

    if (INVALID_SOCKET == ssmtp_connect(sm))
        return sm->errc;
    
    ssmtp_recv(sm, rbuffer, sizeof(rbuffer), 1000);
    if (220 != ssmtp_get_code(rbuffer))
    {
        _shutdown_sock(sm->hSocket, SD_BOTH);
        return sm->errc = CSMTP_SERVER_NOT_READY;
    }
    
    gethostname(localhost, sizeof(localhost)-1);

    // EHLO <SP> <domain> <CRLF>
    sblen = StringCchPrintfA(sbuffer, sizeof(sbuffer), "EHLO %s\r\n", localhost);
    ssmtp_send(sm, sbuffer, sblen);
    ssmtp_recv(sm, rbuffer, sizeof(rbuffer), 1000);

    if (250 != ssmtp_get_code(rbuffer))
    {
        _shutdown_sock(sm->hSocket, SD_BOTH);
        return sm->errc = CSMTP_COMMAND_EHLO;
    }

    _shutdown_sock(sm->hSocket);
	return 0;
}

static SOCKET ssmtp_connect(psimple_smtp sm)
{
    unsigned short port;
    LPHOSTENT hent;
    LPSERVENT sent;
    SOCKADDR_IN sa;
    SOCKET result = INVALID_SOCKET;
    struct in_addr inAddr = {0};

    sm->errc = 0;

    if (isalpha(sm->server[0]))
        hent = gethostbyname(sm->server);
    else
    {
        inAddr.s_addr = inet_addr(sm->server);
        if(inAddr.s_addr == INADDR_NONE) 
        {
            sm->errc = WSAGetLastError();
            result = INVALID_SOCKET;
        } 
        else
            hent = gethostbyaddr((char *) &inAddr, 4, AF_INET);
    }

    if (hent)
    {
        if ((result = socket(PF_INET, SOCK_STREAM,0)) != INVALID_SOCKET)
        {
            if (sm->port != 0)
                port = htons(sm->port);
            else
            {
                sent = getservbyname("mail", 0);
                if (sent == NULL)
                    port = htons(25);
                else 
                    port = sent->s_port;
            }

            sa.sin_family = AF_INET;
            sa.sin_port = port;
            sa.sin_addr = *((LPIN_ADDR)*hent->h_addr_list);

            if (connect(result,(PSOCKADDR)&sa,sizeof(sa)) == SOCKET_ERROR)
            {
                sm->errc = WSAGetLastError();
                _close_sock(result);
            }
        }
        else
        {
            sm->errc = WSAGetLastError();
            result = INVALID_SOCKET;
        }
    }
    else
    {
        sm->errc = WSAGetLastError();
        result = INVALID_SOCKET;
    }

#ifdef _DEBUG
    dbg_printf("SMTP- Connecting to `%s:%d`%c", sm->server, sm->port, (sm->errc ? ' ' : '\n'));
    if (0 != sm->errc)
        dbg_printfc(sm->errc, " %d ", sm->errc);
#endif 

    return sm->hSocket = result;
}

static int ssmtp_get_code(char const* buffer)
{
    if (!buffer)
        return 0;

    return (buffer[0]-'0') * 100 + (buffer[1]-'0') * 10 + (buffer[2]-'0');
}

static int ssmtp_recv(psimple_smtp sm, char* buffer, int len, unsigned timeout)
{
    int result = 0;
    WSABUF buf = { (UINT)len, buffer };
    WSAOVERLAPPED overlapped = {0};

    if (!buffer)
        return 0;

    memset(buffer, 0, 3);
    overlapped.hEvent = WSACreateEvent();

    while (1) 
    {
        int rv = 0;
        int lr = 0;
        DWORD readed = 0;
        DWORD flags = 0;

        Sleep(SsmtpDelayMs);
        rv = WSARecv(sm->hSocket, &buf, 1, &readed, &flags, &overlapped, NULL);
        lr = WSAGetLastError();

        if ((rv == SOCKET_ERROR) && (WSA_IO_PENDING != lr))
            break;

        rv = WSAWaitForMultipleEvents(1, &overlapped.hEvent, TRUE, timeout, TRUE);
        if (rv == WSA_WAIT_FAILED)
            break;

        rv = WSAGetOverlappedResult(sm->hSocket, &overlapped, &readed, FALSE, &flags);
        lr = WSAGetLastError();
        if (rv == FALSE)
            break;

        WSAResetEvent(overlapped.hEvent);

        if (WSA_IO_PENDING != lr)
            break;

        if (readed == 0)
            break;

        if (buf.len > readed)
        {
            buf.buf += readed;
            buf.len -= readed;

            result += readed;
        }
    }

    CloseHandle(overlapped.hEvent);
    return result;
}

static int ssmtp_send(psimple_smtp sm, char const* buffer, int len)
{
    int current = 0;
    int left = len;

    if (!buffer)
        return 0;

    while (left > 0)
    {
        int rv = 0;
        if (rv = send(sm->hSocket, &buffer[current], left, 0) == SOCKET_ERROR)
        {
            sm->errc = WSAGetLastError();
            break;
        }

        if (!rv)
            break;

        left -= rv;
        current += rv;
    }

    dbg_printfc(sm->errc, "SMTP- OUT %s\n%d ", buffer, sm->errc);
    return current;
}

#ifdef _MSC_VER
#  pragma warning(pop)
#endif
#endif
