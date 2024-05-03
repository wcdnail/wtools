#pragma once
#include "redir.h"

class CDemoRedirector :
	public CRedirector
{
public:
	CDemoRedirector(void);
	~CDemoRedirector(void);

public:
	CEdit* m_pWnd;

protected:
	// overrides:
	virtual void WriteStdOut(LPCSTR pszOutput);
	virtual void WriteStdError(LPCSTR pszError);
};
