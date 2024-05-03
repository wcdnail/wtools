#include "StdAfx.h"
#include "GameMenu.h"
#include "MediaManager.h"
#include "resource.h"

CGameMenu::CGameMenu()
	: plasmaConf_(IDD_PLASMACONF)
{
	texA_.Create(256, 256, D3DFMT_A8);
	texA_.CreateSame(texB_);
	texPlasma_.Create(texA_.GetWidth()/2, texA_.GetHeight()/2, D3DFMT_A8R8G8B8);

	genA_.SelectAlgorithm(CONCENTRIC_SHARPEDGE_CIRCLES);
	genA_.SelectTexture(texA_);
	genA_.Generate();

	genB_.SelectAlgorithm(CONCENTRIC_SMOOTH_CIRCLES);
	genB_.SelectTexture(texB_);
	genB_.Generate();

	texBlender_.SelectTexture(texPlasma_, texA_, texB_);
	texBlender_.DefaultPalette(false, false, false, 0xff);
	texBlender_.SpilloverOn();

	plasmaConf_.SetObjects(genA_, genB_, texBlender_);
	plasmaConf_.Show();
	
	RECT rcClient = UI::ClientRect();
	RECT rc	= { 4, 4, 128, 128 };
	UI::Control * pCtl = m_List.Push( new UI::Control );
	pCtl->SetRect( rc );
	pCtl->SetTexture( &texA_ );
	rc.left += 132;
	pCtl = m_List.Push( new UI::Control );
	pCtl->SetRect ( rc );
	pCtl->SetTexture( &texB_ );
	
	RECT rcConfDialog = { 4, 136, 260, rcClient.bottom-132-8 };
	plasmaConf_.Move( rcConfDialog, TRUE );

	rc = rcClient;
	rc.left = 128*2 + 4*3;
	rc.right -= rc.left + 4;
	rc.top += 4;
	rc.bottom -= 4*2;

	pCtl = m_List.Push( new UI::Control );
	pCtl->SetRect ( rc );
	pCtl->SetTexture( &texPlasma_ );
    
	/*
	RECT rc = UI::CenteredRect(75.0f, 95.0f);

	UI::Control * pCtl = m_List.Push( new UI::Control );
	pCtl->SetRect ( rc );
	pCtl->SetTexture( &m_BackTex );

	UI::GrowRect(rc, -10, -10);
	rc.bottom	= (LONG)(rc.bottom * 0.15f);
	LONG dy		= (LONG)(rc.bottom * 0.05f);

	for (int i=0; i<6; i++)
	{
		pCtl = m_List.Push( new UI::Rectangle );
		pCtl->SetColor( 0xff000000 );
		pCtl->SetRect( rc );
		rc.top += (rc.bottom + dy);
	}
	*/
}

void CGameMenu::RenderScene()
{
	texBlender_.Generate();
	CUIScene::RenderScene();
}

