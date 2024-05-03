#pragma once

#include "SmartList.h"
#include "UI.h"

//
//
// Scene, that contained UI elements
//
//

class CUIScene : public IScene
{
public:
	CUIScene();
	~CUIScene();
	void RenderScene();
	void EnterScene();
	void LeaveScene();
	bool SceneDone();
protected:
	typedef CSmartList<UI::Control> ControlList;
	ControlList m_List;
};

//
//
// Scene Manager
//
//

class CSceneManager
{
public:
	CSceneManager();
	~CSceneManager();
	void RenderScene();
	void ChangeScene( IScene* );
private:
	void SetNext();
	IScene *	m_pCurrent;
	IScene *	m_pNext;
	bool		m_bChangeCurrent;
};

extern void ChangeScene(IScene*);
