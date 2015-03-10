#include "Texture.h"
#include <SDL/SDL_image.h>

std::string GetFileExtension(const std::string& FileName)
{
	if (FileName.find_last_of(".") != std::string::npos)
		return FileName.substr(FileName.find_last_of(".") + 1);
	return "";
}

CTexture::CTexture()
{
	mipmap = false;
	loaded = false;
	file = "";
}

CTexture::CTexture(std::string file, bool generateMipMap)
{
	Load_2D(file, generateMipMap);
}

bool CTexture::Load_2D(std::string file, bool generateMipMap)
{
	if (loaded) Release();
	this->mipmap = generateMipMap;
	this->file = file;
	std::string ext = GetFileExtension(file);
	if (ext == "dds")
	{
		return load_DDS(file);
	}
	else
	{
		return load_SDL(file);
	}
}


//Constants for compressed textures
#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII
bool CTexture::load_DDS(std::string file)
{
	unsigned char header[124];

	FILE *fp;

	/* try to open the file */
	fp = fopen(file.c_str(), "rb");
	if (fp == NULL){
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", file); getchar();
		return 0;
	}

	/* verify the type of file */
	char filecode[4];
	fread(filecode, 1, 4, fp);
	if (strncmp(filecode, "DDS ", 4) != 0) {
		printf("Wrong extension!\n");
		fclose(fp);
		return 0;
	}

	/* get the surface desc */
	fread(&header, 124, 1, fp);

	unsigned int height = *(unsigned int*)&(header[8]);
	unsigned int width = *(unsigned int*)&(header[12]);
	unsigned int linearSize = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC = *(unsigned int*)&(header[80]);


	unsigned char * buffer;
	unsigned int bufsize;
	/* how big is it going to be including all mipmaps? */
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
	buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
	fread(buffer, 1, bufsize, fp);
	/* close the file pointer */
	fclose(fp);

	unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
	unsigned int format;
	switch (fourCC)
	{
	case FOURCC_DXT1:
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case FOURCC_DXT3:
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case FOURCC_DXT5:
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		free(buffer);
		return 0;
	}

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;

	/* load the mipmaps */
	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
	{
		unsigned int size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
			0, size, buffer + offset);

		offset += size;
		width /= 2;
		height /= 2;

		// Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
		if (width < 1) width = 1;
		if (height < 1) height = 1;

	}

	free(buffer);

	loaded = true;
	return true;
}

bool CTexture::load_SDL(std::string file)
{
	SDL_Surface* Surf_Load = NULL;
	if (GetFileExtension(file) == "bmp")
		Surf_Load = SDL_LoadBMP(file.c_str());
	else
		Surf_Load = IMG_Load(file.c_str());

	if (Surf_Load == NULL)
	{
		Error("Texture Loading Error", "Error loading: " + file + "\nError message: " + std::string(IMG_GetError()));
		return false;
	}

	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	GLint nOfColors = Surf_Load->format->BytesPerPixel;
	GLenum imgFormat;

	//Get output format
	if (nOfColors == 4)     // contains an alpha channel
	{
		format = GL_RGBA;
	}
	else if (nOfColors == 3)     // no alpha channel
	{
		format = GL_RGB;
	}
	else if (nOfColors == 1)
	{
		format = GL_LUMINANCE;
	}
	else {
		printf("warning: the image is not truecolor..  this will probably break\n");
	}
	//Get input format
	if (nOfColors == 4)
	{
		if (Surf_Load->format->Rmask == 255)
			imgFormat = GL_RGBA;
		else
			imgFormat = GL_BGRA;
	}
	else
	{
		if (Surf_Load->format->Rmask == 255)
			imgFormat = GL_RGB;
		else
			imgFormat = GL_BGR;
	}
	//Create the actual openGL texture
	glTexImage2D(GL_TEXTURE_2D, 0, format, Surf_Load->w, Surf_Load->h, 0, imgFormat, GL_UNSIGNED_BYTE, Surf_Load->pixels);

	width = Surf_Load->w;
	height = Surf_Load->h;
	BPP = Surf_Load->format->BytesPerPixel;

	if (mipmap){
		glGenerateMipmap(GL_TEXTURE_2D);	//Generate mipmaps
	}

	glGenSamplers(1, &sampler);

	SDL_FreeSurface(Surf_Load);

	loaded = true;
	return true;
}

void CTexture::CreateEmpty(int width, int height, GLenum format)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	if (format == GL_RGBA || format == GL_BGRA)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
	// We must handle this because of internal format parameter
	else if (format == GL_RGB || format == GL_BGR)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, NULL);

	glGenSamplers(1, &sampler);
}

void CTexture::CreateFromData(BYTE* data, int width, int height, int BPP, GLenum format, bool generateMipMap)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	if (format == GL_RGBA || format == GL_BGRA)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	// We must handle this because of internal format parameter
	else if (format == GL_RGB || format == GL_BGR)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	if (generateMipMap)
		glGenerateMipmap(GL_TEXTURE_2D);
	glGenSamplers(1, &sampler);

	this->file = "";
	this->mipmap = generateMipMap;
	this->width = width;
	this->height = height;
	this->BPP = BPP;
}

void CTexture::SetWrap(int wrapS, int wrapT)
{
	glBindSampler(0, sampler);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, wrapS);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, wrapT);
}

void CTexture::SetSamplerParameter(GLenum parameter, GLenum value)
{
	glSamplerParameteri(sampler, parameter, value);
}

void CTexture::SetFiltering(int magnification, int minification)
{
	glBindSampler(0, sampler);

	// Set magnification filter
	if (magnification == TEXTURE_FILTER_MAG_NEAREST)
		glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	else if (magnification == TEXTURE_FILTER_MAG_BILINEAR)
		glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Set minification filter
	if (minification == TEXTURE_FILTER_MIN_NEAREST)
		glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	else if (minification == TEXTURE_FILTER_MIN_BILINEAR)
		glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	else if (minification == TEXTURE_FILTER_MIN_NEAREST_MIPMAP)
		glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	else if (minification == TEXTURE_FILTER_MIN_BILINEAR_MIPMAP)
		glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	else if (minification == TEXTURE_FILTER_MIN_TRILINEAR)
		glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	minification = minification;
	magnification = magnification;
}

void CTexture::Bind(int textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindSampler(textureUnit, sampler);
}

void CTexture::Release()
{
	glDeleteSamplers(1, &sampler);
	glDeleteTextures(1, &texture);
}

bool CTexture::Reload()
{
	if (!loaded)	return false;
	return Load_2D(file, mipmap);
}

int CTexture::GetMagnificationFilter()
{
	return magnification;
}

int CTexture::GetMinificationFilter()
{
	return minification;
}

int CTexture::GetWidth()
{
	return width;
}

int CTexture::GetHeight()
{
	return height;
}

int CTexture::GetBPP()
{
	return BPP;
}

GLuint CTexture::GetID()
{
	return texture;
}

std::string CTexture::GetFile()
{
	return file;
}