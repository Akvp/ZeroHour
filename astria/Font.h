#ifndef _FONT_SDL_H_
#define _FONT_SDL_H_

#include "Common.h"
#include "Shader.h"
#include "VBO.h"

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
	bool Load(Font_SDL* font, std::string text, SDL_Color color, TTF_RENDER_TYPE type);
private:
	GLuint Texture;

	GLuint VAO;
	CVBO VBO;

	CShaderProgram* Program;
};
#endif