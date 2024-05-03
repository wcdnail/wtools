#pragma once

#include "SceneManagment.h"
#include "GameMenu.h"

class CMainLayout
{
public:
	CMainLayout();
	~CMainLayout();

	void CreateObjects();

private:
	CGameMenu * m_pGameMenu;
};
