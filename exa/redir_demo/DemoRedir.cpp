#include "StdAfx.h"
#include "demoredir.h"

CDemoRedirector::CDemoRedirector(void)
{
	m_pWnd = NULL;
}

CDemoRedirector::~CDemoRedirector(void)
{
}

void CDemoRedirector::WriteStdOut(LPCSTR pszOutput)
{
	if (m_pWnd != NULL)
	{
		int nSize = m_pWnd->GetWindowTextLength();
		m_pWnd->SetSel(nSize, nSize);
		m_pWnd->ReplaceSel(pszOutput);		// add the message to the end of Edit control
	}
}

void CDemoRedirector::WriteStdError(LPCSTR pszError)
{
	WriteStdOut("\r\nError message: ");
	WriteStdOut(pszError);
}
