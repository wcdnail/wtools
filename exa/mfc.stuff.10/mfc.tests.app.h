#pragma once

#include "res/resource.h"

class Application: public CWinApp
{
public:
	Application();
	virtual BOOL InitInstance();

private:
	DECLARE_MESSAGE_MAP()
};

extern Application gApp;
