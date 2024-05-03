#pragma once

class IDrawable
{
public:
	virtual void Display()				= 0;
	virtual void SetRect(const RECT&)	= 0;
	virtual RECT GetRect() const		= 0;
};

template <class T>
class IListElement
{
public:
	void	Next	(T * pNext) { m_pNext = pNext; }
	T *		Next	()	const	{ return m_pNext; }
	T **	NextPtr	()			{ return &m_pNext; }
protected:
	IListElement() : m_pNext( NULL ) { }
	T *	m_pNext;
};

class IScene
{
public:
	virtual void RenderScene()			= 0;
	virtual void EnterScene()			= 0;
	virtual void LeaveScene()			= 0;
	virtual bool SceneDone()			= 0;
};
