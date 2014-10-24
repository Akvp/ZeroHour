#ifndef _APP_STATE_PAUSE_H_
#define _APP_STATE_PAUSE_H_

#include "AppState.h"
#include "Texture_SDL.h"

class CAppStatePause
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
	static CAppStatePause Instance;
	
	static const int NUM_OPTIONS = 5;

	Texture_SDL snapshot;
	Texture_SDL menu;
	Texture_SDL options[NUM_OPTIONS];
};

#endif