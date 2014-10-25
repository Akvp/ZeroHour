#include "Font_SDL.h"

Font_SDL::Font_SDL()
{
	font = NULL;
}

bool Font_SDL::Load(const char* file, int ptsize)
{
	if ((font = TTF_OpenFont(file, ptsize)) == NULL)
		return false;
}

void Font_SDL::SetStyle(int style)
{
	TTF_SetFontStyle(font, style);
}

int Font_SDL::GetStyle()
{
	return TTF_GetFontStyle(font);
}

void Font_SDL::SetOutline(int outline)
{
	TTF_SetFontOutline(font, outline);
}

void Font_SDL::EnableKerning()
{
	TTF_SetFontKerning(font, 1);
}

void Font_SDL::DisableKerning()
{
	TTF_SetFontKerning(font, 0);
}

void Font_SDL::Release()
{
	TTF_CloseFont(font);
}

TTF_Font* Font_SDL::GetFont()
{
	return font;
}

// =====================================================

Text_SDL::Text_SDL()
{
	Renderer = NULL;
	FontTexture = NULL;
}

bool Text_SDL::Load(TTF_Font* font, std::string text, SDL_Renderer* renderer, SDL_Color color, TTF_RENDER_TYPE type)
{
	Renderer = renderer;
	
	SDL_Surface* Surf_Load = NULL;
	
	switch (type)
	{
	case TTF_SOLID:
		Surf_Load = TTF_RenderText_Solid(font, text.c_str(), color);
		break;
	case TTF_SHADED:
		Surf_Load = TTF_RenderText_Shaded(font, text.c_str(), color, { 1, 1, 1, 1 });
		break;
	case TTF_BLENDED:
		Surf_Load = TTF_RenderText_Blended(font, text.c_str(), color);
		break;
	}
	if (Surf_Load == NULL)
		return false;

	if ((FontTexture = SDL_CreateTextureFromSurface(Renderer, Surf_Load)) == NULL)
		return false;

	SDL_QueryTexture(FontTexture, NULL, NULL, &width, &height);
	return true;
}

void Text_SDL::Render(int x, int y)
{
	SDL_Rect dest = { x, y, width, height };
	SDL_RenderCopy(Renderer, FontTexture, NULL, &dest);
}

void Text_SDL::Render(int x, int y, float ratio)
{
	SDL_Rect dest = { x, y, width*ratio, height*ratio };
	SDL_RenderCopy(Renderer, FontTexture, NULL, &dest);
}

void Text_SDL::Release()
{
	SDL_DestroyTexture(FontTexture);
	FontTexture = NULL;
	Renderer = NULL;
}

int Text_SDL::GetWidth()
{
	return width;
}

int Text_SDL::GetHeight()
{
	return height;
}