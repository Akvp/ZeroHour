#ifndef _APP_STATE_INTRO_H_
#define _APP_STATE_INTRO_H_

#include "AppState.h"

class CAppStateIntro : public CAppState
{
public:
	CAppStateIntro();
	static CAppStateIntro* GetInstance();

	void OnActivate();
	void OnDeactivate();

	void OnEvent(SDL_Event* Event);
	void OnLoop();
	void OnRender();

private:
	static CAppStateIntro Instance;
	int StartTime;
};

#endif