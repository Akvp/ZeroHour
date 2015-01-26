#ifndef _APP_STATE_PAUSE_H_
#define _APP_STATE_PAUSE_H_

#include "AppState.h"
#include "Texture_SDL.h"
#include "Font.h"
#include "Main.h"

enum PAUSE_MENU
{
	PAUSE_MENU_RESUME,
	PAUSE_MENU_CREDITS,	
	PAUSE_MENU_ABOUT,
	PAUSE_MENU_HELP,
	PAUSE_MENU_EXIT,
	PAUSE_MENU_COUNT,
	PAUSE_MENU_NONE
};

class CAppStatePause : public CAppState
{
public:
	CAppStatePause();

	static CAppStatePause* GetInstance();

	void OnActivate();
	void OnDeactivate();
	void OnExit();

	void OnEvent(SDL_Event* Event);
	void OnUpdate();
	void OnRender();

private:
	void OnKeyDown(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode);
	void OnLButtonDown(int mX, int mY);

	void OnSelect();

private:
	static CAppStatePause Instance;
	bool loaded;

	PAUSE_MENU Selection;

	Font_SDL Font;
	Text_SDL Menu[PAUSE_MENU_COUNT];
	Text_SDL MenuHover[PAUSE_MENU_COUNT];

	int OffsetX, OffsetY;
	int MouseX, MouseY;
};

#endif