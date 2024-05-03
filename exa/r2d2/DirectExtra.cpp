#include "StdAfx.h"
#include "DirectExtra.h"
#include "ErrorReport.h"

#define Case(N)	case N: return IdentToString(N)

const TCHAR * D3DSwapEffectToString(D3DSWAPEFFECT SwapEffect)
{
	switch (SwapEffect)
	{
		Case(D3DSWAPEFFECT_DISCARD);
		Case(D3DSWAPEFFECT_FLIP);
		Case(D3DSWAPEFFECT_COPY);
		Case(D3DSWAPEFFECT_FORCE_DWORD);
	}

	return NULL;
}

const TCHAR *D3DFormatToString(D3DFORMAT Format)
{
	switch (Format)
	{
		Case(D3DFMT_R8G8B8);
		Case(D3DFMT_A8R8G8B8);
		Case(D3DFMT_X8R8G8B8);
		Case(D3DFMT_R5G6B5);
		Case(D3DFMT_X1R5G5B5);
		Case(D3DFMT_A1R5G5B5);
		Case(D3DFMT_A4R4G4B4);
		Case(D3DFMT_R3G3B2);
		Case(D3DFMT_A8);
		Case(D3DFMT_A8R3G3B2);
		Case(D3DFMT_X4R4G4B4);
		Case(D3DFMT_A2B10G10R10);
		Case(D3DFMT_A8B8G8R8);
		Case(D3DFMT_X8B8G8R8);
		Case(D3DFMT_G16R16);
		Case(D3DFMT_A2R10G10B10);
		Case(D3DFMT_A16B16G16R16);
		Case(D3DFMT_A8P8);
		Case(D3DFMT_P8);
		Case(D3DFMT_L8);
		Case(D3DFMT_A8L8);
		Case(D3DFMT_A4L4);
		Case(D3DFMT_V8U8);
		Case(D3DFMT_L6V5U5);
		Case(D3DFMT_X8L8V8U8);
		Case(D3DFMT_Q8W8V8U8);
		Case(D3DFMT_V16U16);
		Case(D3DFMT_A2W10V10U10);
		Case(D3DFMT_UYVY);
		Case(D3DFMT_R8G8_B8G8);
		Case(D3DFMT_YUY2);
		Case(D3DFMT_G8R8_G8B8);
		Case(D3DFMT_DXT1);
		Case(D3DFMT_DXT2);
		Case(D3DFMT_DXT3);
		Case(D3DFMT_DXT4);
		Case(D3DFMT_DXT5);
		Case(D3DFMT_D16_LOCKABLE);
		Case(D3DFMT_D32);
		Case(D3DFMT_D15S1);
		Case(D3DFMT_D24S8);
		Case(D3DFMT_D24X8);
		Case(D3DFMT_D24X4S4);
		Case(D3DFMT_D16);
		Case(D3DFMT_D32F_LOCKABLE);
		Case(D3DFMT_D24FS8);
		Case(D3DFMT_L16);
		Case(D3DFMT_VERTEXDATA);
		Case(D3DFMT_INDEX16);
		Case(D3DFMT_INDEX32);
		Case(D3DFMT_Q16W16V16U16);
		Case(D3DFMT_MULTI2_ARGB8);
		Case(D3DFMT_R16F);
		Case(D3DFMT_G16R16F);
		Case(D3DFMT_A16B16G16R16F);
		Case(D3DFMT_R32F);
		Case(D3DFMT_G32R32F);
		Case(D3DFMT_A32B32G32R32F);
		Case(D3DFMT_CxV8U8);
		Case(D3DFMT_FORCE_DWORD);
	}

	return IdentToString( D3DFMT_UNKNOWN );
}

const TCHAR * D3DPoolToString(D3DPOOL Pool)
{
	switch (Pool)
	{
		Case(D3DPOOL_DEFAULT);
		Case(D3DPOOL_MANAGED);
		Case(D3DPOOL_SYSTEMMEM);
		Case(D3DPOOL_SCRATCH);
		Case(D3DPOOL_FORCE_DWORD);
	}

	return NULL;
}

void AddLogD3DPresentParameters(const D3DPRESENT_PARAMETERS& d3dpp)
{
	AddLogMessage( L"windowed                  : %s", ( d3dpp.Windowed ? L"yes" : L"no" ) );
	AddLogMessage( L"swap effect               : %s ( %d )", D3DSwapEffectToString(d3dpp.SwapEffect), d3dpp.SwapEffect );
	AddLogMessage( L"back buffer format        : %s ( %d )", D3DFormatToString(d3dpp.BackBufferFormat), d3dpp.BackBufferFormat);
	AddLogMessage( L"back buffer width         : %d", d3dpp.BackBufferWidth );
	AddLogMessage( L"back buffer height        : %d", d3dpp.BackBufferHeight );
	AddLogMessage( L"back buffer count         : %d", d3dpp.BackBufferCount );
	AddLogMessage( L"auto depth stencil        : %s", ( d3dpp.EnableAutoDepthStencil ? L"yes" : L"no" ) );
	AddLogMessage( L"auto depth stencil format : %s ( %d )", D3DFormatToString(d3dpp.AutoDepthStencilFormat), d3dpp.AutoDepthStencilFormat );
}
