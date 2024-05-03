#include "StdAfx.h"
#include "SceneManagment.h"
#include "Application.h"

void ChangeScene(IScene * pNewScene) 
{ 
	GetApp().GetManager().ChangeScene( pNewScene ); 
}

//
//
// UI Scene
//
//

CUIScene::CUIScene() {}
CUIScene::~CUIScene() {}
void CUIScene::EnterScene() {}
void CUIScene::LeaveScene() {}
bool CUIScene::SceneDone()	{ return true; }
void CUIScene::RenderScene()
{
	m_List.ForEach<ControlList::PFN_voidvoid>(&UI::Control::Display);
}

//
//
// Scene Manager
//
//

CSceneManager::CSceneManager()
	: m_pCurrent		( NULL )
	, m_pNext			( NULL )
	, m_bChangeCurrent	( false )
{
}

CSceneManager::~CSceneManager()
{
}

void CSceneManager::ChangeScene( IScene * pScene )
{
	if (m_pCurrent) m_pCurrent->LeaveScene();
	m_bChangeCurrent = true;
	m_pNext = pScene;
}

void CSceneManager::SetNext()
{
	if (m_pNext)
	{
		m_pCurrent = m_pNext;
		m_pCurrent->EnterScene();
		m_pNext = NULL;
		m_bChangeCurrent = false;
	}
}

void CSceneManager::RenderScene()
{
	if (m_pCurrent) m_pCurrent->RenderScene();

	if (m_bChangeCurrent)
	{
		if (m_pCurrent) { if ( m_pCurrent->SceneDone() ) SetNext(); }
		else { SetNext(); }
	}
}
