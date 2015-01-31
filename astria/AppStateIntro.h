#ifndef _APP_STATE_INTRO_H_
#define _APP_STATE_INTRO_H_

#include "AppState.h"
#include "Texture_SDL.h"
#include "Main.h"
#include "Font.h"

enum INTRO_MENU
{
	INTRO_MENU_START,
	INTRO_MENU_ABOUT,
	INTRO_MENU_HELP,
	INTRO_MENU_CREDITS,
	INTRO_MENU_EXIT,
	INTRO_MENU_COUNT,
	INTRO_MENU_NONE
};

class CAppStateIntro : public CAppState
{
public:
	static CAppStateIntro* GetInstance();

	void OnActivate();
	void OnDeactivate();

	void OnEvent(SDL_Event* Event);
	void OnUpdate();
	void OnRender();

private:
	void LoadFontTextures();
	
	void OnLButtonDown(int mX, int mY);
	void OnSelect();

private:
	static CAppStateIntro Instance;

	INTRO_MENU Selection;

	Font_SDL Font;
	Text_SDL IntroText;
	Text_SDL Menu[INTRO_MENU_COUNT];
	Text_SDL MenuHover[INTRO_MENU_COUNT];

	int MouseX;
	int MouseY;

	int OffsetX, OffsetY;

	Texture_SDL splash;
};

#endif