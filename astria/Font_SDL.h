#ifndef _FONT_SDL_H_
#define _FONT_SDL_H_

#include "Common.h"

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

	bool Load(TTF_Font* font, std::string text, SDL_Renderer* renderer, TTF_RENDER_TYPE type = TTF_BLENDED);

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

#endif