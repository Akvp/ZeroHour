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
	void Load(std::string a_sDirectory);
	void Render();

	void Release();

	CTexture* GetTextures() { return tTextures; };

private:
	UINT uiVAO;
	CVBO vboRenderData;
	CTexture tTextures[6];
	std::string sDirectory;
	std::string sFront, sBack, sLeft, sRight, sTop, sBottom;
	GLuint tex_cube;
};

#endif