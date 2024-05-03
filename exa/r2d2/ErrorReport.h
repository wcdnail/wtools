#pragma once

extern bool NoErrors();
extern void AddLogMessage(LPCWSTR, ...);
extern void AddWarningMessage(LPCWSTR, ...);
extern void AddErrorMessage(LPCWSTR, ...);
extern void AddNoticeMessage(LPCWSTR, ...);
extern void ShowMessages();

#define LM_BEG_TEXT			L"Вход     "
#define LM_END_TEXT			L"Выход    "
#define LM_ERROR_TEXT		 "Ошибка   ";
#define LM_WARNING_TEXT		 "Внимание ";
#define LM_NOTICE_TEXT		 ">>>>     ";

#ifdef _NEED_CALL_TRACER
namespace addin
{
	class CFnName
	{
	public:
		CFnName(char * szFnName) 
			: m_pszFnName(szFnName) 
					{ AddLogMessage( LM_BEG_TEXT L"%S", szFnName ); }
		~CFnName()	{ AddLogMessage( LM_END_TEXT L"%S", m_pszFnName ); }
	private:
		char * m_pszFnName;

	};
}
#define EnterFunc() addin::CFnName __FuncEnt( __FUNCTION__ )
#else	// !_NEED_CALL_TRACER
#define EnterFunc() 
#endif	// _NEED_CALL_TRACER

