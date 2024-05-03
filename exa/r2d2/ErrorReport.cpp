#include "StdAfx.h"
#include "ErrorReport.h"
#include "Application.h"

#define _USE_STL

#ifdef _USE_STL
#include <fstream>
using namespace std;
#endif

enum eLogBufLen
{ 
	MAX_LOG_BUFFER_LEN = 512 
};

enum eLogMsgType
{
	LOG_MSG_NORM = 0,
	LOG_MSG_ERROR,
	LOG_MSG_WARNING,
	LOG_MSG_NOTICE
};

const char	szErrorMarker[]		= LM_ERROR_TEXT;
const char	szWarningMarker[]	= LM_WARNING_TEXT;
const char	szNoticeMarker[]	= LM_NOTICE_TEXT;
char		pszErrorLogName[]	= "ErrorLog.temp";
unsigned	g_nErrors			= 0;
unsigned	g_nWarnings			= 0;
unsigned	g_nMaxWarnings		= 10;

bool NoErrors() { return 0 == g_nErrors; }

namespace addin
{
	class CLogCleaner
	{
	public: 
		CLogCleaner() {}
		~CLogCleaner() 
		{ 
			::DeleteFileA( pszErrorLogName );	
		}
	} LogCrearer;
}

void DropToLog( const char * pszChars, bool bEndLine = true )
{
#ifdef _USE_STL
	ofstream slog;
	slog.open( pszErrorLogName, ios_base::out | ios_base::app );
	slog << pszChars;
	if (bEndLine) slog << endl;
	slog.flush();
	slog.close();
#else
	FILE * slog = fopen( pszErrorLogName, "a" );
	if (slog)
	{
		const char *  pszFormat = "%s";
		if (bEndLine) pszFormat = "%s\n";
		fprintf( slog, pszFormat, pszChars );
		fflush( slog );
		fclose( slog );
	}
#endif
}

void AddFormatMessage(LPCWSTR pszFormat, va_list pArgs)
{
	wchar_t szBuff[ MAX_LOG_BUFFER_LEN ];
	::_vsnwprintf( szBuff, MAX_LOG_BUFFER_LEN-1, pszFormat, pArgs );
	szBuff[ MAX_LOG_BUFFER_LEN-1 ] = 0;

	char szChars[ MAX_LOG_BUFFER_LEN ];
	::WideCharToMultiByte( CP_ACP, 0, szBuff, MAX_LOG_BUFFER_LEN, szChars, MAX_LOG_BUFFER_LEN, NULL, NULL );

	DropToLog( szChars );
}

void AddLogMessage(LPCWSTR pszFormat, ...)
{
	va_list pArgs;
	va_start( pArgs, pszFormat );
	AddFormatMessage( pszFormat, pArgs );
	va_end( pArgs );
}

void AddErrorMessage(LPCWSTR pszFormat, ...)
{
	DropToLog( szErrorMarker, false );

	va_list pArgs;
	va_start( pArgs, pszFormat );
	AddFormatMessage( pszFormat, pArgs );
	va_end( pArgs );

	++g_nErrors;
}

void AddWarningMessage(LPCWSTR pszFormat, ...)
{
	DropToLog( szWarningMarker, false );

	va_list pArgs;
	va_start( pArgs, pszFormat );
	AddFormatMessage( pszFormat, pArgs );
	va_end( pArgs );

	++g_nWarnings;
	if ( g_nWarnings > g_nMaxWarnings ) 
		AddErrorMessage( L"слишком много предупреждений" );
}

void AddNoticeMessage(LPCWSTR pszFormat, ...)
{
	DropToLog( szNoticeMarker, false );

	va_list pArgs;
	va_start( pArgs, pszFormat );
	AddFormatMessage( pszFormat, pArgs );
	va_end( pArgs );
}

template <class T>
inline void RemoveControlChars( T * pszBuffer )
{
	if (pszBuffer)
	{
		T * p = pszBuffer;
		while ( *p )
		{
			if ( *p < 32 ) { *p = 32; }
			++p;
		}
	}
}


LRESULT CALLBACK LoggerProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	static HWND hList = NULL;
	enum { BACK, SEL };
	static HBRUSH hBrush[2];
	static COLORREF nTextColor[4];
	static UINT nItemHeight = 16;
	static int nErrorCode = 0;

	const COLORREF nBackColor = 0x00000000;
	const COLORREF nSelColor = RGB(0x70, 0x70, 0x70);
	const COLORREF nNormColor = RGB(0xff, 0xff, 0xff);
	const COLORREF nWarningColor = RGB(0xff, 0xff, 0);
	const COLORREF nErrorColor = RGB(0xff, 0, 0);
	const COLORREF nNoticeColor = RGB(0x00, 0xff, 0x00);

	switch ( uMsg )
	{
	case WM_INITDIALOG:

		hList = ::GetDlgItem( hWnd, IDC_LIST );
		if ( !hList ) 
		{
            ::EndDialog( hWnd, -1 );
            return 0;
		}
		else
		{
#ifdef _USE_STL 
			ifstream slog;
			slog.open( pszErrorLogName );
            if (slog.bad())
            {
                ::EndDialog( hWnd, -2 );
                return 0;
            }

			slog.seekg( 0, ios::beg );

            char szBuffer[ MAX_LOG_BUFFER_LEN ];
            size_t szBufferLen = 0;

			while ( !slog.eof() )
			{
				slog.getline( szBuffer, 1024 );
				szBufferLen = strlen( szBuffer );

#else	// ! _USE_STL

			FILE * slog = fopen( pszErrorLogName, "r" );
			if ( !slog ) { nErrorCode = -2; goto ClosingDialog; }

			while ( !feof( slog ) )
			{
				if ( !fgets( szBuffer, MAX_LOG_BUFFER_LEN - 1, slog ) ) break;
				szBufferLen = strlen( szBuffer );
				szBuffer[ szBufferLen - 1 ] = 0;

#endif	// _USE_STL

				if ( szBufferLen > 1 )
				{
					UINT nIndex = (UINT)::SendMessageA( hList, LB_ADDSTRING, 0, (LPARAM)szBuffer );
					UINT nMessageType = LOG_MSG_NORM;

					if		(strstr(szBuffer, szErrorMarker))	nMessageType = LOG_MSG_ERROR;
					else if (strstr(szBuffer, szWarningMarker)) nMessageType = LOG_MSG_WARNING;
					else if (strstr(szBuffer, szNoticeMarker))	nMessageType = LOG_MSG_NOTICE;

					::SendMessageA( hList, LB_SETITEMDATA, nIndex, (LPARAM)nMessageType );
				}
			}
#ifndef _USE_STL 
			fclose( slog );
#endif
		}

		hBrush	[ BACK	] = ::CreateSolidBrush( nBackColor );
		hBrush	[ SEL	] = ::CreateSolidBrush( nSelColor );

		nTextColor	[ LOG_MSG_NORM		] = nNormColor;
		nTextColor	[ LOG_MSG_WARNING	] = nWarningColor;
		nTextColor	[ LOG_MSG_ERROR		] = nErrorColor;
		nTextColor	[ LOG_MSG_NOTICE	] = nNoticeColor;

		return 1;

	case WM_CTLCOLORLISTBOX:
		return (LRESULT)hBrush[ BACK ];

	case WM_MEASUREITEM:
	{
		LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT) lParam; 
		if (IDC_LIST == wParam)	lpmis->itemHeight = nItemHeight; 
		return TRUE;
	}

	case WM_DRAWITEM:
	{
		LPDRAWITEMSTRUCT lpdis;
		lpdis = (LPDRAWITEMSTRUCT) lParam;

		if ( IDC_LIST == wParam )
		{
			UINT nMessageType = LOG_MSG_NORM;
			TEXTMETRICA	tm;
			size_t		pcch;
			char		szItemText[ MAX_LOG_BUFFER_LEN ];

			int x = lpdis->rcItem.left, 
				y = lpdis->rcItem.top, 
				w = lpdis->rcItem.right - x, 
				h = lpdis->rcItem.bottom - y;

			::SendMessageA( lpdis->hwndItem, LB_GETTEXT, lpdis->itemID, (LPARAM) szItemText ); 
			nMessageType = (UINT)::SendMessageA( lpdis->hwndItem, LB_GETITEMDATA, lpdis->itemID, 0 );

			::GetTextMetricsA( lpdis->hDC, &tm );
			::StringCchLengthA( szItemText, MAX_LOG_BUFFER_LEN, &pcch );

			nItemHeight = tm.tmHeight;

			if ( lpdis->itemState & ODS_SELECTED )
				::SelectObject( lpdis->hDC, hBrush[ SEL ] );
			else
				::SelectObject( lpdis->hDC, hBrush[ BACK ] );

			::PatBlt( lpdis->hDC, x, y, w, h, PATCOPY );
			::SetTextColor( lpdis->hDC, nTextColor[ nMessageType ] );
			::SetBkMode( lpdis->hDC, TRANSPARENT );
			::TextOutA(lpdis->hDC, x + 4, y, szItemText, (int)pcch);
		}

		break;
	}
		
	case WM_COMMAND:
		if ( (IDOK == wParam) || (IDCANCEL == wParam) ) 
		{
			::DeleteObject( hBrush[ BACK ] );
			::DeleteObject( hBrush[ SEL ] );
			::EndDialog( hWnd, nErrorCode );
		}
		break;
	}
	return 0;
}

void ShowMessages()
{
	::ShowWindow( GetApp().WindowHandle(), SW_HIDE );

	::DialogBox( GetApp().ModuleHandle()
		, MAKEINTRESOURCE( IDD_LOGGER )
		, NULL
		, (DLGPROC)LoggerProc );
}
