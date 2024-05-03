#pragma once

#include "Base.h"
#include "SmartList.h"
#include "Texture.h"

class CMediaManager
{
public:
	CMediaManager();
	~CMediaManager();

	void		AddPath		( LPCWSTR );
	bool		FindFile	( LPCWSTR, bool (CMediaManager::*)(LPCWSTR, void **), void ** );
	DXTexture *	LoadTexture	( LPCWSTR );
	
private:
	bool OnTextureFind(LPCWSTR, void **);

	struct StringItem: public IListElement<StringItem>
	{
		~StringItem() { if (m_bDelete) SAFE_DELETE_ARRAY(m_pString); }
		LPCWSTR		m_pString;
		bool		m_bDelete;
	};

	struct TextureItem: public IListElement<TextureItem>
	{
		~TextureItem() { SAFE_DELETE( m_pTex ); }
		DXTexture *	m_pTex;
		enum		{ NameSize = 32 - sizeof( DXTexture* )  };
		wchar_t		m_szName[ NameSize ];
	};

	void AddSeparatorIfNessesary(StringItem&);
	TextureItem * FindTexture(LPCWSTR);
	
	CSmartList<StringItem>		m_Path;
	CSmartList<TextureItem>		m_Textures;
};

inline DXTexture * CMediaManager::LoadTexture(LPCWSTR pszName)
{
	DXTexture * pResult = NULL;
	FindFile( pszName, &CMediaManager::OnTextureFind, (void**)&pResult );
	return pResult;
}

extern CMediaManager & Media();
