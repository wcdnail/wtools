#ifndef _crash_report_smtp_h__
#define _crash_report_smtp_h__

#if 0
#include <wchar.h>

#ifdef _WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef _WIN32_WINNT
#    define _WIN32_WINNT 0x0500
#  endif 
#  include <windows.h>
#  include <winsock2.h>
#  include <winerror.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum simple_smtp_errors
{
#ifdef _WIN32
    ErrorInvalidArgs = ERROR_INVALID_PARAMETER,
#endif
    CSMTP_NO_ERROR = 0,
    CSMTP_WSA_STARTUP = 100,
    CSMTP_WSA_VER,
    CSMTP_WSA_SEND,
    CSMTP_WSA_RECV,
    CSMTP_WSA_CONNECT,
    CSMTP_WSA_GETHOSTBY_NAME_ADDR,
    CSMTP_WSA_INVALID_SOCKET,
    CSMTP_WSA_HOSTNAME,
    CSMTP_BAD_IPV4_ADDR,
    CSMTP_UNDEF_MSG_HEADER = 200,
    CSMTP_UNDEF_MAILFROM,
    CSMTP_UNDEF_SUBJECT,
    CSMTP_UNDEF_RECIPENTS,
    CSMTP_UNDEF_LOGIN,
    CSMTP_UNDEF_PASSWORD,
    CSMTP_UNDEF_RECIPENT_MAIL,
    CSMTP_COMMAND_MAIL_FROM = 300,
    CSMTP_COMMAND_EHLO,
    CSMTP_COMMAND_AUTH_LOGIN,
    CSMTP_COMMAND_DATA,
    CSMTP_COMMAND_QUIT,
    CSMTP_COMMAND_RCPT_TO,
    CSMTP_MSG_BODY_ERROR,
    CSMTP_CONNECTION_CLOSED = 400,
    CSMTP_SERVER_NOT_READY,
    CSMTP_FILE_NOT_EXIST,
    CSMTP_MSG_TOO_BIG,
    CSMTP_BAD_LOGIN_PASS,
    CSMTP_UNDEF_XYZ_RESPOMSE,
    CSMTP_LACK_OF_MEMORY,
};

enum simple_smtp_misc
{
    SsmtpPriorityHigh = 2,
    SsmtpPriorityNormal = 3,
    SsmtpPriorityLow = 4,
    SsmtpNeedWsaStartup = 0x0001,
};

typedef struct _simple_smtp
{
    int errc;
    char const* server;
    unsigned short port;
    char const* m_pcLogin;
    char const* m_pcPassword;
    char const* m_pcMailFrom;
    char const* m_pcNameFrom;
    char const* m_pcSubject;
    wchar_t const* m_pcMsgBody;
    char const* m_pcXMailer;
    char const* m_pcReplyTo;
    char const* m_pcIPAddr;
    int m_iXPriority;

#ifdef _WIN32
    SOCKET hSocket;
    int wsaStartup;
    WSADATA wsaData;
#endif
} 
simple_smtp, *psimple_smtp;

int simple_smtp_init(psimple_smtp sm, unsigned flags);
void simple_smtp_delete(psimple_smtp sm);
int simple_smtp_send(psimple_smtp sm);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

#endif /* _crash_report_smtp_h__ */
