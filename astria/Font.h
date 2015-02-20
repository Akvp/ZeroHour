#ifndef _FONT_SDL_H_
#define _FONT_SDL_H_

#include "Common.h"
#include "Shader.h"
#include "VBO.h"
#include "Texture.h"
#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef _DEBUG
#pragma comment(lib, "freetype255d.lib")
#else
#pragma comment(lib, "freetype255.lib")
#endif

enum TTF_RENDER_TYPE
{
	TTF_SOLID,
	TTF_SHADED,
	TTF_BLENDED
};

class Font_SDL
{
public:
	Font_SDL();

	bool Load(const char* file, int ptsize);

	//Style can be 
	//TTF_STYLE_BOLD
	//TTF_STYLE_ITALIC
	//TTF_STYLE_UNDERLINE
	//TTF_STYLE_STRIKETHROUGH
	void SetStyle(int style);
	int GetStyle();

	void SetOutline(int outline);

	void EnableKerning();
	void DisableKerning();

	void Release();

	TTF_Font* GetFont();
private:
	TTF_Font* font;
};

class Text_SDL
{
public:
	Text_SDL();

	bool Load(Font_SDL* font, std::string text, SDL_Renderer* renderer, SDL_Color color = { 0, 0, 0, 1 }, TTF_RENDER_TYPE type = TTF_BLENDED);

	void Render(int x, int y);
	void Render(int x, int y, float ratio);

	void Release();

	int GetWidth();
	int GetHeight();
private:
	SDL_Renderer* Renderer;
	SDL_Texture* FontTexture;

	int width;
	int height;
};

class Text_GL
{
public:
	Text_GL();

	bool LoadFont(std::string file, int size);
	bool LoadSystemFont(std::string font, int size);

	int GetTextWidth(std::string text, int size);

	void Print(std::string text, int x, int y, int size = -1);
	void PrintFormatted(int x, int y, int size, char* text, ...);

	void Release();
	void SetShader(CShaderProgram* program);

	//For debugging
#ifdef _DEBUG
	CTexture* GetTexture() { return CharTextures; }
#endif

private:
	void CreateChar(int index);

	CTexture CharTextures[256];
	int AdvX[256], AdvY[256];
	int BearingX[256], BearingY[256];
	int CharW[256], CharH[256];
	int PixelSize, NewLine;

	bool Loaded;

	GLuint VAO;
	CVBO vboData;

	FT_Library Library;
	FT_Face Face;
	CShaderProgram* ProgramFont;
};
#endif