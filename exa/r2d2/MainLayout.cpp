#include "StdAfx.h"
#include "MainLayout.h"
#include "MediaManager.h"

CMainLayout::CMainLayout()
	: m_pGameMenu ( NULL )
{
}

CMainLayout::~CMainLayout()
{
}

void CMainLayout::CreateObjects()
{
	UI::InitControls();

	Media().AddPath( L"Paks\\" );
	Media().AddPath( L"Paks\\menu" );
	Media().AddPath( L"Paks\\graphics\\" );
	
	m_pGameMenu = new CGameMenu();
	::ChangeScene( m_pGameMenu );
}
