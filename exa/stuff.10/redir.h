//////////////////////////////////////////////////////////////////////
//
// Redirector - to redirect the input / output of a console
//
// Developer: Jeff Lee
// Dec 10, 2001
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CRedirector
{
public:
	CRedirector();
	~CRedirector();

private:
	HANDLE m_hThread;		// thread to receive the output of the child process
	HANDLE m_hEvtStop;		// event to notify the redir thread to exit
	DWORD m_dwThreadId;		// id of the redir thread
	DWORD m_dwWaitTime;		// wait time to check the status of the child process

protected:
	HANDLE m_hStdinWrite;	// write end of child's stdin pipe
	HANDLE m_hStdoutRead;	// read end of child's stdout pipe
	HANDLE m_hChildProcess;

	BOOL LaunchChild(LPCTSTR pszCmdLine, HANDLE hStdOut, HANDLE hStdIn, HANDLE hStdErr);
	int RedirectStdout();
	void DestroyHandle(HANDLE& rhObject);

	static DWORD WINAPI OutputThread(LPVOID lpvThreadParam);

protected:
	// overrides:
	virtual void WriteStdOut(PCSTR pszOutput);
	virtual void WriteStdError(PCSTR pszError);

public:
	BOOL Open(LPCTSTR pszCmdLine);
	virtual void Close();
	BOOL Printf(LPCTSTR pszFormat, ...);

	void SetWaitTime(DWORD dwWaitTime) { m_dwWaitTime = dwWaitTime; }
};
