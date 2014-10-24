#ifndef _APP_STATE_INTRO_H_
#define _APP_STATE_INTRO_H_

#include "AppState.h"
#include "Texture_SDL.h"

class CAppStateIntro : public CAppState
{
public:
	CAppStateIntro();
	static CAppStateIntro* GetInstance();

	void OnActivate();
	void OnDeactivate();

	void OnEvent(SDL_Event* Event);
	void OnUpdate();
	void OnRender();

private:
	void OnLButtonDown(int mX, int mY);

private:
	static CAppStateIntro Instance;
	int StartTime;
	Texture_SDL splash;
};

#endif