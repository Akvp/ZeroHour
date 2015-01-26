#include "Font.h"
#include "Main.h"

Font_SDL::Font_SDL()
{
	font = NULL;
}

bool Font_SDL::Load(const char* file, int ptsize)
{
	if ((font = TTF_OpenFont(file, ptsize)) == NULL)
		return false;
	return true;
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

bool Text_SDL::Load(Font_SDL* font, std::string text, SDL_Renderer* renderer, SDL_Color color, TTF_RENDER_TYPE type)
{
	Renderer = renderer;
	
	SDL_Surface* Surf_Load = NULL;
	
	switch (type)
	{
	case TTF_SOLID:
		Surf_Load = TTF_RenderText_Solid(font->GetFont(), text.c_str(), color);
		break;
	case TTF_SHADED:
		Surf_Load = TTF_RenderText_Shaded(font->GetFont(), text.c_str(), color, { 1, 1, 1, 1 });
		break;
	case TTF_BLENDED:
		Surf_Load = TTF_RenderText_Blended(font->GetFont(), text.c_str(), color);
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

void RenderText(std::string message, SDL_Color color, int x, int y, int size) 
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	gluOrtho2D(0, CMain::GetInstance()->GetWindowWidth(), 0, CMain::GetInstance()->GetWindowHeight()); // m_Width and m_Height is the resolution of window
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	TTF_Font * font = TTF_OpenFont("ttf/complex.ttf", size);
	SDL_Surface * sFont = TTF_RenderText_Blended(font, message.c_str(), color);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sFont->w, sFont->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, sFont->pixels);

	glBegin(GL_QUADS);
	{
		glTexCoord2f(0, 0); glVertex2f(x, y);
		glTexCoord2f(1, 0); glVertex2f(x + sFont->w, y);
		glTexCoord2f(1, 1); glVertex2f(x + sFont->w, y + sFont->h);
		glTexCoord2f(0, 1); glVertex2f(x, y + sFont->h);
	}
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glDeleteTextures(1, &texture);
	TTF_CloseFont(font);
	SDL_FreeSurface(sFont);
}