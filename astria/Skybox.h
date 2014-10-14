#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "Texture.h"
#include "VBO.h"

/********************************

Class:		CSkybox

Purpose:	Class for using skybox.

********************************/

class CSkybox
{
public:
	void load(std::string a_sDirectory, std::string a_sFront, std::string a_sBack, std::string a_sLeft, std::string a_sRight, std::string a_sTop, std::string a_sBottom);
	void render();

	void release();

	CTexture* getTextures() { return tTextures; };
private:
	UINT uiVAO;
	CVBO vboRenderData;
	CTexture tTextures[6];
	std::string sDirectory;
	std::string sFront, sBack, sLeft, sRight, sTop, sBottom;
};

#endif