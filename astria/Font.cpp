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

//=========================================================================

Text_GL::Text_GL()
{

}

bool Text_GL::Load(Font_SDL* font, std::string text, SDL_Color color, TTF_RENDER_TYPE type = TTF_BLENDED)
{
	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);

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
	{
		Error("Error loading font", "Error loading font\nError code: " + string(TTF_GetError()));
		return false;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Surf_Load->w, Surf_Load->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, Surf_Load->pixels);

	glm::vec2 vQuad[] =
	{
		glm::vec2(0.0f, Surf_Load->h),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(Surf_Load->w, Surf_Load->h),
		glm::vec2(Surf_Load->w, 0)
	};
	glm::vec2 vTexQuad[] = { glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f) };
	for (int i = 0; i < 4; i++)
	{
		VBO.AddData(&vQuad[i], sizeof(glm::vec2));
		VBO.AddData(&vTexQuad[i], sizeof(glm::vec2));
	}

	VBO.UploadGPU(GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2) * 2, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2) * 2, (void*)(sizeof(glm::vec2)));

	SDL_FreeSurface(Surf_Load);

	return true;
}