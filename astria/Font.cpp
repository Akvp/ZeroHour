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
	Loaded = false;
}

inline int next_p2(int n){ int res = 1; while (res < n)res <<= 1; return res; }

void Text_GL::CreateChar(int index)
{
	FT_Load_Glyph(Face, FT_Get_Char_Index(Face, index), FT_LOAD_DEFAULT);

	FT_Render_Glyph(Face->glyph, FT_RENDER_MODE_NORMAL);
	FT_Bitmap* pBitmap = &Face->glyph->bitmap;

	int iW = pBitmap->width, iH = pBitmap->rows;
	int iTW = next_p2(iW), iTH = next_p2(iH);

	GLubyte* bData = new GLubyte[iTW*iTH];
	// Copy glyph data and add dark pixels elsewhere
	for (int ch = 0; ch < iTH; ch++)
	{
		for (int cw = 0; cw < iTW; cw++)
		{
			bData[ch*iTW + cw] = (ch >= iH || cw >= iW) ? 0 : pBitmap->buffer[(iH - ch - 1)*iW + cw];
		}
	}

	// And create a texture from it

	CharTextures[index].CreateFromData(bData, iTW, iTH, 8, GL_DEPTH_COMPONENT, false);
	CharTextures[index].SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR);

	CharTextures[index].SetSamplerParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	CharTextures[index].SetSamplerParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Calculate glyph data
	AdvX[index] = Face->glyph->advance.x >> 6;
	BearingX[index] = Face->glyph->metrics.horiBearingX >> 6;
	CharW[index] = Face->glyph->metrics.width >> 6;

	AdvY[index] = (Face->glyph->metrics.height - Face->glyph->metrics.horiBearingY) >> 6;
	BearingY[index] = Face->glyph->metrics.horiBearingY >> 6;
	CharH[index] = Face->glyph->metrics.height >> 6;

	NewLine = max(NewLine, int(Face->glyph->metrics.height >> 6));

	// Rendering data, texture coordinates are always the same, so now we waste a little memory
	glm::vec2 vQuad[] =
	{
		glm::vec2(0.0f, float(-AdvY[index] + iTH)),
		glm::vec2(0.0f, float(-AdvY[index])),
		glm::vec2(float(iTW), float(-AdvY[index] + iTH)),
		glm::vec2(float(iTW), float(-AdvY[index]))
	};
	glm::vec2 vTexQuad[] = { glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f) };

	// Add this char to VBO
	for (int i = 0; i < 4; i++)
	{
		vboData.AddData(&vQuad[i], sizeof(glm::vec2));
		vboData.AddData(&vTexQuad[i], sizeof(glm::vec2));
	}
	delete[] bData;
}

bool Text_GL::LoadFont(string file, int pixelsize)
{
	if (FT_Init_FreeType(&Library) != 0)
	{
		Error("Font loading error", "Error loading font library");
		return false;
	}

	if (FT_New_Face(Library, file.c_str(), 0, &Face) != 0)
	{
		Error("Font loading", "Error loading font: " + file);
		return false;
	}

	FT_Set_Pixel_Sizes(Face, pixelsize, pixelsize);
	PixelSize = pixelsize;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	vboData.Create();
	vboData.Bind();

	for (int i = 0; i < 128; i++)	CreateChar(i);
	Loaded = true;

	FT_Done_Face(Face);
	FT_Done_FreeType(Library);

	vboData.UploadGPU(GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2) * 2, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2) * 2, (void*)(sizeof(glm::vec2)));
	return true;
}

bool Text_GL::LoadSystemFont(string font, int pixelsize)
{
	char buf[512]; GetWindowsDirectory(buf, 512);
	string Path = buf;
	Path += "\\Fonts\\";
	Path += font;

	return LoadFont(Path, pixelsize);
}

int Text_GL::GetTextWidth(string text, int pixelsize)
{
	int Result = 0;
	for (int i = 0; i < text.size(); i++) Result += AdvX[text[i]];

	return Result*pixelsize / PixelSize;
}

void Text_GL::Print(string text, int x, int y, int pixelsize)
{
	if (!Loaded)return;

	glBindVertexArray(VAO);
	ProgramFont->SetUniform("gSampler", 0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int iCurX = x, iCurY = y;
	if (pixelsize == -1)pixelsize = PixelSize;
	float fScale = float(pixelsize) / float(PixelSize);
	for (int i = 0; i < text.size(); i++)
	{
		if (text[i] == '\n')
		{
			iCurX = x;
			iCurY -= NewLine*pixelsize / PixelSize;
			continue;
		}
		int iIndex = int(text[i]);
		iCurX += BearingX[iIndex] * pixelsize / PixelSize;
		if (text[i] != ' ')
		{
			CharTextures[iIndex].Bind();
			glm::mat4 mModelView = glm::translate(glm::mat4(1.0f), glm::vec3(float(iCurX), float(iCurY), 0.0f));
			mModelView = glm::scale(mModelView, glm::vec3(fScale));
			ProgramFont->SetUniform("matrices.modelViewMatrix", mModelView);
			// Draw character
			glDrawArrays(GL_TRIANGLE_STRIP, iIndex * 4, 4);
		}

		iCurX += (AdvX[iIndex] - BearingX[iIndex])*pixelsize / PixelSize;
	}
	glDisable(GL_BLEND);
}

void Text_GL::PrintFormatted(int x, int y, int pixelsize, char* sText, ...)
{
	char buf[512];
	va_list ap;
	va_start(ap, sText);
	vsprintf(buf, sText, ap);
	va_end(ap);
	Print(buf, x, y, pixelsize);
}

void Text_GL::Release()
{
	for (int i = 0; i < 128; i++)	CharTextures[i].Release();
	vboData.Release();
	glDeleteVertexArrays(1, &VAO);
}

void Text_GL::SetShader(CShaderProgram* program)
{
	ProgramFont = program;
}