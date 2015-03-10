#include "FBO.h"

CFBO::CFBO()
{
	FrameBuffer = NULL;
	DepthBuffer = NULL;
}

bool CFBO::CreateDepthShadowBuffer()
{
	const char* LOG_TAG = "FBO::CreateDepthShadowBuffer";
	glGenFramebuffers(1, &FrameBuffer);
	
	Texture.CreateEmpty(1024, 1024, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT16, GL_FLOAT, 0);
	Texture.SetFiltering(TEXTURE_FILTER_MAG_NEAREST, TEXTURE_FILTER_MIN_NEAREST);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, Texture.GetID(), 0);
	glDrawBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Error(LOG_TAG, "glCheckFramebufferStatus failed");
		return false;
	}
}

bool CFBO::CreateWithTexture(int width, int height)
{
	const char* LOG_TAG = "FBO::CreateWithTexture";
	if (FrameBuffer != 0)
	{
		Error(LOG_TAG, "Trying to create into an existing buffer");
		return false;
	}

	glGenFramebuffers(1, &FrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

	Texture.CreateEmpty(width, height, GL_RGB);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Texture.GetID(), 0);

	Width = width;
	Height = height;

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Error(LOG_TAG, "glCheckFramebufferStatus failed");
		return false;
	}

	return true;
}

bool CFBO::AddDepthBuffer()
{
	if (FrameBuffer == 0) return false;

	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

	glGenRenderbuffers(1, &DepthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, DepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, Width, Height);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthBuffer);
	return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void CFBO::Bind(bool fullViewPort)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
	if (fullViewPort) glViewport(0, 0, 1024, 1024);
}

void CFBO::BindShadowMap()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
	glViewport(0, 0, 1024, 1024);
}

void CFBO::SetTextureFiltering(int mag, int min)
{
	Texture.SetFiltering(mag, min);
	Texture.SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

void CFBO::BindTexture(int textureUnit, bool mipmaps)
{
	Texture.Bind(textureUnit);
	if (mipmaps) glGenerateMipmap(GL_TEXTURE_2D);
}

void CFBO::Release()
{
	if (FrameBuffer)
	{
		glDeleteFramebuffers(1, &FrameBuffer);
		FrameBuffer = 0;
	}
	if (DepthBuffer)
	{
		glDeleteRenderbuffers(1, &DepthBuffer);
		DepthBuffer = 0;
	}
	Texture.Release();
}

glm::mat4 CFBO::GetProjectionMatrix(float a_fov, float a_near, float a_far)
{
	return glm::perspective(a_fov, float(Width) / float(Height), a_near, a_far);
}

glm::mat4 CFBO::GetOrthogonalMatrix()
{
	return glm::ortho(0.0f, float(Width), 0.0f, float(Height));
}

int CFBO::GetWidth()
{
	return Width;
}

int CFBO::GetHeight()
{
	return Height;
}