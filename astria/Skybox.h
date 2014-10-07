#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include <gl/glew.h>

#include "Texture.h"
#include "VBO.h"

class CSkybox
{
public:
	void load(char* front, char* back, char* left, char* right, char* top, char* bottom);
	void render();

	void release();

private:
	GLuint VAO;
	CVBO vboRenderData;
	CTexture textures[6];
	char* front;
	char* back;
	char* left;
	char* right;
	char* top;
	char* bottom;
};

#endif