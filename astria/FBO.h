#ifndef _FBO_H_
#define _FBO_H_

#include "Common.h"
#include "Texture.h"

class CFBO
{
public:
	CFBO::CFBO();

	bool Create();

	void Bind(bool fullViewPort = true);
	void BindShadowMap();

	void SetTextureFiltering(int mag, int min);
	void BindTexture(int textureUnit = 0, bool mipmaps = false);

	void Release();

	bool CreateDepthShadowBuffer();
	bool CreateWithTexture(int width, int height);

	bool AddDepthBuffer();

	glm::mat4 GetProjectionMatrix(float a_fov, float a_near, float a_far);
	glm::mat4 GetOrthogonalMatrix();
	int GetWidth();
	int GetHeight();
private:
	GLuint FrameBuffer;
	GLuint DepthBuffer;
	CTexture Texture;
	int Width, Height;
};

#endif