#ifndef _APP_STATE_PAUSE_H_
#define _APP_STATE_PAUSE_H_

#include "AppState.h"
#include "Texture_SDL.h"
#include "Font_SDL.h"
#include "Main.h"

enum PAUSE_MENU
{
	PAUSE_MENU_OPTIONS,
	PAUSE_MENU_EXIT,	
	PAUSE_MENU_ABOUT,
	PAUSE_MENU_HELP,
	PAUSE_MENU_COUNT,
	PAUSE_MENU_NONE
};

struct Button
{
	int x, y, w, h;
};

class CAppStatePause : public CAppState
{
public:
	CAppStatePause();

	static CAppStatePause* GetInstance();

	void OnActivate();
	void OnDeactivate();

	void OnEvent(SDL_Event* Event);
	void OnUpdate();
	void OnRender();

private:
	void OnKeyDown(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode);
	
private:
	static CAppStatePause Instance;

	PAUSE_MENU selection;

	Font_SDL complex;
	Text_SDL options[PAUSE_MENU_COUNT];
};

#endif