#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "Common.h"

enum ETextureFiltering
{
	TEXTURE_FILTER_MAG_NEAREST = 0, // Nearest criterion for magnification
	TEXTURE_FILTER_MAG_BILINEAR, // Bilinear criterion for magnification
	TEXTURE_FILTER_MIN_NEAREST, // Nearest criterion for minification
	TEXTURE_FILTER_MIN_BILINEAR, // Bilinear criterion for minification
	TEXTURE_FILTER_MIN_NEAREST_MIPMAP, // Nearest criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_BILINEAR_MIPMAP, // Bilinear criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_TRILINEAR, // Bilinear criterion for minification on two closest mipmaps, then averaged
};

class CTexture
{
public:
	CTexture();
	CTexture(std::string file, bool generateMipMap = false);

	bool Load_2D(std::string file, bool generateMipMap = false);
	void CreateEmpty(int width, int height, GLenum format);
	void CreateFromData(BYTE* data, int width, int height, int BPP, GLenum format, bool generateMipMap = false);
	void Release();

	bool Reload();
	
	void SetFiltering(int magnification, int minification);
	void SetSamplerParameter(GLenum parament, GLenum value);

	int GetMinificationFilter();
	int GetMagnificationFilter();

	int GetWidth();
	int GetHeight();
	int GetBPP();

	std::string GetFile();

	void Bind(int textureUnit = 0);

	GLuint GetID();
	GLuint operator()();

private:
	bool load_SDL(std::string file);
	bool load_DDS(std::string file);

private:
	bool loaded;

	std::string file;
	GLuint texture;
	GLuint sampler;
	GLenum format;
	int width, height, BPP;
	bool mipmap;

	int minification;
	int magnification;
};

#endif