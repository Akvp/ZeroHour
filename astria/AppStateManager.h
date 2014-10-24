#ifndef _APP_STATE_MANAGER_H_
#define _APP_STATE_MANAGER_H_

#include "AppState.h"

//List of all app states
#include "AppStateMain.h"
#include "AppStateIntro.h"
#include "AppStatePause.h"

enum
{
	APPSTATE_NONE,
	APPSTATE_INTRO,
	APPSTATE_MAIN,
	APPSTATE_PAUSE
};

class CAppStateManager
{
private:
	static CAppState* ActiveAppState;

public:
	static void OnEvent(SDL_Event* Event);
	static void OnUpdate();
	static void OnRender();

	static void SetActiveAppState(int AppStateID);
	static CAppState* GetActiveAppState();
};

#endif