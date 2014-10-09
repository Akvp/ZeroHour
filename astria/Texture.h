#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <gl/glew.h>
#include <Windows.h>
#include <string>

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
	CTexture(const char* file, bool generateMipMap = false);

	bool load_2D(const char* file, bool generateMipMap = false);
	void createEmpty(int width, int height, GLenum format);
	void createFromData(BYTE* data, int width, int height, int BPP, GLenum format, bool generateMipMap = false);
	void release();

	bool reload();
	
	void setFiltering(int magnification, int minification);
	void setSamplerParameter(GLenum parament, GLenum value);

	int getMinificationFilter();
	int getMagnificationFilter();

	int getWidth();
	int getHeight();
	int getBPP();

	const char* getFile();

	void bind(int textureUnit = 0);

	GLuint getID();
	GLuint operator()();

private:
	bool load_SDL(const char* file);
	bool load_DDS(const char* file);

private:
	const char* file;
	GLuint texture;
	GLuint sampler;
	int width, height, BPP;
	bool mipmap;

	int minification;
	int magnification;
};

#endif