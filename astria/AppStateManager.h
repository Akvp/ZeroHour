#ifndef _APP_STATE_MANAGER_H_
#define _APP_STATE_MANAGER_H_

#include "AppState.h"

//List of all app states

enum
{
	APPSTATE_NONE,
	APPSTATE_MAIN,
	APPSTATE_PAUSEMENU
};

class CAppStateManager
{
private:
	static CAppState* ActiveAppState;

	static void OnEvent(SDL_Event* Event);
	static void OnLoop();
	static void OnRender();

	static void SetActiveAppState(int AppStateID);
	static CAppState* GetActiveAppState();
};

#endif