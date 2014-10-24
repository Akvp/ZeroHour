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
	void load(std::string a_sDirectory);
	void render();

	void release();

	CTexture* getTextures() { return tTextures; };

private:
	void load_texture(std::string file, GLenum side);

private:
	UINT uiVAO;
	CVBO vboRenderData;
	CTexture tTextures[6];
	std::string sDirectory;
	std::string sFront, sBack, sLeft, sRight, sTop, sBottom;
	GLuint tex_cube;
};

#endif