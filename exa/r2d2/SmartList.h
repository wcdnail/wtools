#pragma once

typedef const RECT& CTRECTref;

template <class T>
class CSmartList
{
public:
	CSmartList();
	~CSmartList();
	T *		Push		( T * );
	void	Clear		();
	T *		Begin		()	const		{ return m_pList; }
	void	SelfDestruct( bool IsIt )	{ m_bSelfDestruct = IsIt; }

	typedef void (T::*PFN_voidvoid)(void);
	typedef void (T::*PFN_void4float)(float, float, float, float);
	typedef void (T::*PFN_voidcrectref)(CTRECTref);

	template <class U> struct Caller 
	{ 
		typedef U Type;
		Type m_pFunc;
		Caller ( Type pFunc ) : m_pFunc( pFunc ) {}
		void Invoke( T*, va_list ) { }
	};

	template <> struct Caller<PFN_voidvoid>
	{
		typedef PFN_voidvoid Type;
		Type m_pFunc;
		Caller ( Type pFunc ) : m_pFunc( pFunc ) {}
		void Invoke( T* pItem, va_list ) { ((*pItem).*(m_pFunc))(); }
	}; 

	template <> struct Caller<PFN_void4float>
	{
		typedef PFN_void4float Type;
		Type m_pFunc;
		Caller ( Type pFunc ) : m_pFunc( pFunc ) {}
		void Invoke( T* pItem, va_list marker ) 
		{ 
			double * Value = (double*) marker;
			((*pItem).*(m_pFunc))(
				  (float)Value[0]
				, (float)Value[1]
				, (float)Value[2]
				, (float)Value[3] );
		}
	}; 

	template <class cT>
	void ForEach( Caller<cT> Pointee, ... )
	{
		va_list mark;
		va_start( mark, Pointee );

		T * pItem = m_pList;
		while ( pItem )
		{
			Pointee.Invoke( pItem, mark );
			pItem = pItem->Next();
		}

		va_end( mark );
	}

protected:
	T  *	m_pList;
	T **	m_ppItem;
	bool	m_bSelfDestruct;
};

template <class T>
inline CSmartList<T>::CSmartList()
	: m_pList			( NULL )
	, m_ppItem			( &m_pList )
	, m_bSelfDestruct	( true )
{
}

template <class T>
inline CSmartList<T>::~CSmartList()
{
	if ( m_bSelfDestruct ) Clear();
}

template <class T>
inline void CSmartList<T>::Clear()
{
	T * pItem = m_pList;
	while ( pItem )
	{
		T * pToDelete = pItem;
		pItem = pItem->Next();
		delete pToDelete;
	}
}

template <class T>
inline T * CSmartList<T>::Push( T * pNew )
{
	if (pNew)
	{
		*m_ppItem = pNew;
		 m_ppItem = pNew->NextPtr();
	}
	return pNew;
}
