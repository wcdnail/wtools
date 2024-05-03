#include "StdAfx.h"
#include "MediaManager.h"
#include "ErrorReport.h"

CMediaManager & Media()
{
	static CMediaManager MediaManager;
	return MediaManager;
}

CMediaManager::CMediaManager() {}

CMediaManager::~CMediaManager() {}

void CMediaManager::AddPath(LPCWSTR pszPath)
{
	if (pszPath)
	{
		StringItem * pStr = m_Path.Push( new StringItem );
		pStr->m_pString  = pszPath;
		pStr->m_bDelete	 = false;
		AddSeparatorIfNessesary( *pStr );
	}
}

void CMediaManager::AddSeparatorIfNessesary(StringItem& Str) 
{
	size_t nLen = ::wcslen( Str.m_pString );
	wchar_t C = Str.m_pString[ nLen-1 ];

	if ( (C ==  L'\\') || (C == L'/') ) return;
	
	TCHAR * pNewString = new TCHAR [ nLen + 2 ];
	::wcsncpy( pNewString, Str.m_pString, nLen );
	pNewString[ nLen ] = L'\\';
	pNewString[ nLen + 1 ] = 0;
	Str.m_pString = pNewString;
	Str.m_bDelete = true;
}

bool CMediaManager::FindFile( LPCWSTR pszName
							 , bool (CMediaManager::*pfnOnFind)(LPCWSTR, void **)
							 , void ** ppTarget )
{
	if ( pszName )
	{
		StringItem * pPath = m_Path.Begin();
		while ( pPath )
		{
			size_t nLen = ::wcslen( pPath->m_pString ) + ::wcslen( pszName );
			wchar_t * pszFullName = new wchar_t [ nLen + 1 ];
			::wcscpy( pszFullName, pPath->m_pString );
			::wcscat( pszFullName, pszName );

			FILE * Checked = _wfopen( pszFullName, L"r" );
			if ( Checked ) fclose( Checked );

			bool rval = false;
			if ( Checked && pfnOnFind ) 
				rval = !((*this).*(pfnOnFind))( pszFullName, ppTarget );

			delete [] pszFullName;
			if ( Checked ) return rval;
			pPath = pPath->Next();
		}
	}

	AddErrorMessage( L"файл не найден %s", pszName );
	return false;
}

CMediaManager::TextureItem * CMediaManager::FindTexture(LPCWSTR pszFullName)
{
    TextureItem * pItem = m_Textures.Begin();
	while ( pItem )
	{
		if (! ::wcsncmp( pItem->m_szName, pszFullName, TextureItem::NameSize-1 ) )
		{
			return pItem;
		}
		pItem = pItem->Next();
	}

	return NULL;
}

bool CMediaManager::OnTextureFind(LPCWSTR pszFullName, void ** ppTargetTexturePtr )
{
	TextureItem * pAlreadyIn = FindTexture(pszFullName);
	if ( pAlreadyIn )
	{
		*(DXTexture **)ppTargetTexturePtr = pAlreadyIn->m_pTex;
		return true;
	}

	TextureItem * pItem = new TextureItem;

	pItem->m_pTex = new DXTexture;
	pItem->m_pTex->Load(pszFullName);

	if ( pItem->m_pTex->Good() )
	{
		::wcsncpy( pItem->m_szName, pszFullName, TextureItem::NameSize - 2 );
		pItem->m_szName[ TextureItem::NameSize - 1 ] = 0;

		m_Textures.Push( pItem );
		*(DXTexture **)ppTargetTexturePtr = pItem->m_pTex;
		return true;
	}

	delete pItem->m_pTex;
	pItem->m_pTex = NULL;
	delete pItem;
	return false;
}
