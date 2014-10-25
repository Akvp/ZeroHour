//==============================================================================
/*
	Texture class for wrapping SDL Textures

	3/13/2014
    SDLTutorials.com
    Tim Jones
*/
//==============================================================================

// Modified

#ifndef __TEXTURE_H__
	#define __TEXTURE_H__

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <string>

class Texture_SDL {
	private:
		std::string Filename;

		int Width = 0;
		int Height = 0;

		SDL_Renderer* Renderer = NULL;
		SDL_Texture* SDLTexture = NULL;

	public:
		Texture_SDL();
		~Texture_SDL();

		bool Load(SDL_Renderer* Renderer, std::string Filename);

		void Render(int X, int Y);
		void Render(int X, int Y, int Width, int Height);
		void Render(int X, int Y, int Width, int Height, int SX, int SY, int SWidth, int SHeight);
		void Render(int X, int Y, double Angle);
		void Render(int X, int Y, int Width, int Height, double Angle);

		void Release();

		void SetAlpha(Uint8 alpha);
		void SetBlend(SDL_BlendMode blend);

		int GetWidth();
		int GetHeight();
		SDL_Texture* GetTexture();
};

#endif
