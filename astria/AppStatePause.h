#ifndef _APP_STATE_PAUSE_H_
#define _APP_STATE_PAUSE_H_

#include "AppState.h"
#include "Texture_SDL.h"
#include "Font_SDL.h"
#include "MenuTree.h"
#include "Main.h"

//For now, pause menu uses SDL 2.0 instead of OpenGL
//Will try change to OpenGL later... if I happen to have time

typedef void(*fnptr)();

enum PAUSE_MENU
{
	PAUSE_MENU_RESUME,
	PAUSE_MENU_OPTIONS,	
	PAUSE_MENU_ABOUT,
	PAUSE_MENU_HELP,
	PAUSE_MENU_EXIT,
	PAUSE_MENU_COUNT,
	PAUSE_MENU_NONE
};

enum PAUSE_SUBMENU_OPTIONS
{
	PAUSE_SUBMENU_OPTIONS_RESOLUTION,
	PAUSE_SUBMENU_OPTIONS_FULLSCREEN,
	PAUSE_SUBMENU_OPTIONS_COUNT,
	PAUSE_SUBMENU_OPTIONS_NONE
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
	void LoadFontTextures();
	void UnloadFontTextures();

	void OnKeyDown(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode);
	void OnLButtonDown(int mX, int mY);

	void OnSelect();

private:
	static CAppStatePause Instance;
	bool loaded;

	PAUSE_MENU selection;

	Font_SDL font;
	Text_SDL menu[PAUSE_MENU_COUNT];
	Text_SDL menu_hover[PAUSE_MENU_COUNT];

	int mouseX;
	int mouseY;
	
//private:	
//	MenuNode* PauseMenu;
//	MenuNode* CurrentMenu;
};

#endif