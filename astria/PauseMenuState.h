#ifndef _PAUSE_MENU_STATE_H_
#define _PAUSE_MENU_STATE_H_

#include "AppState.h"

enum OPTION_SELECTION
{
	OPTION_SELECTION_RESOLUTION,
	OPTION_SELECTION_FULLSCREEN,
	OPTION_SELECTION_COUNT,
	OPTION_SELECTION_NONE
};

class CPauseStateOptions : public CAppState
{
public:
	void OnActivate();
	void OnDeactivate();

	void OnEvent(SDL_Event* Event);
	void OnUpdate();
	void OnRender();

private:
	OPTION_SELECTION options[OPTION_SELECTION_COUNT];

	//Event variables and functions
	int mouseX;
	int mouseY;
	void OnKeyDown(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode);
	void OnLButtonDown();
};

#endif