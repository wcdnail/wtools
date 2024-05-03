#pragma once

#include "SceneManagment.h"
#include "TextureGenerator.h"
#include "TextureShaderGenerator.h"
#include "TextureGeneratorConf.h"

class CGameMenu: public CUIScene
{
public:
	CGameMenu();
    ~CGameMenu() {}

	void RenderScene();
private:

	DXTexture                texA_;
	DXTexture                texB_;
	DXTexture                texPlasma_;
	TexGen::PlasmaCircles   genA_;
	TexGen::PlasmaCircles   genB_;
	TexGen::PlasmaBlender   texBlender_;
	TexGen::ConfigDialog    plasmaConf_;
};

