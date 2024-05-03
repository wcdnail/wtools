#include "stdafx.h"
#include "Texture.h"
#include "ErrorReport.h"
#include "DirectExtra.h"
#include "Application.h"

typedef HRESULT (*PD3DXCreateTexture)
    (
    _In_   LPDIRECT3DDEVICE9 pDevice,
    _In_   UINT Width,
    _In_   UINT Height,
    _In_   UINT MipLevels,
    _In_   DWORD Usage,
    _In_   D3DFORMAT Format,
    _In_   D3DPOOL Pool,
    _Out_  LPDIRECT3DTEXTURE9 *ppTexture
    );

typedef HRESULT (*PD3DXCreateTextureFromFile)
    (
    _In_   LPDIRECT3DDEVICE9 pDevice,
    _In_   LPCTSTR pSrcFile,
    _Out_  LPDIRECT3DTEXTURE9 *ppTexture
    );

PD3DXCreateTexture pD3DXCreateTexture = NULL;
PD3DXCreateTextureFromFile pD3DXCreateTextureFromFile = NULL;

void DXTexture::Create(UINT nWidth, UINT nHeight, D3DFORMAT d3dFormat, D3DPOOL d3dPool, DWORD dwUsage)
{
    if (NULL != pD3DXCreateTexture)
    {
        SAFE_RELEASE(tex_);

        IDirect3DDevice9 *pDev = GetApp().GetD3DDevice();

	    HRESULT hr = pD3DXCreateTexture( pDev, nWidth, nHeight, 1, dwUsage, d3dFormat, d3dPool, &tex_ );
	    if ( FAILED(hr) )
	    {
		    AddErrorMessage( L"создания текстуры <%dx%d %s %s>"
			    , nWidth
			    , nHeight
			    , D3DFormatToString(d3dFormat)
			    , D3DPoolToString(d3dPool)
			    );
	    }
    }
}

void DXTexture::Load(LPCWSTR pszFileName)
{
    if (NULL != pD3DXCreateTextureFromFile)
    {
        SAFE_RELEASE(tex_);

        ::AddNoticeMessage( L"попытка загрузки <%s>", pszFileName );
        IDirect3DDevice9 * pDev = GetApp().GetD3DDevice();
        HRESULT hr;
        hr = pD3DXCreateTextureFromFile( pDev, pszFileName, &tex_ );
        if ( FAILED(hr) )
        {
            AddErrorMessage( L"загрузки текстуры <%s>", pszFileName );
        }
    }
}

bool DXTexture::CreateSame(DXTexture& target) const
{
	if (tex_ && !target.Good())
	{
		D3DSURFACE_DESC d3dSurfaceDesc;
		if ( SUCCEEDED(tex_->GetLevelDesc(0, &d3dSurfaceDesc)) )
		{
			target.Create(d3dSurfaceDesc.Width, d3dSurfaceDesc.Height, d3dSurfaceDesc.Format
				, d3dSurfaceDesc.Pool, d3dSurfaceDesc.Usage);
		}
	}

	return target.Good();
}

UINT DXTexture::GetWidth() const
{
	UINT rv = 0;
	D3DSURFACE_DESC ddsd;
	if (tex_ && SUCCEEDED(tex_->GetLevelDesc( 0, &ddsd )))  rv = ddsd.Width;
	return rv;
}

UINT DXTexture::GetHeight() const
{
	UINT rv = 0;
	D3DSURFACE_DESC ddsd;
	if (tex_ && SUCCEEDED(tex_->GetLevelDesc(0, &ddsd))) rv = ddsd.Height;
	return rv;
}
