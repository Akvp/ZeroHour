#ifndef _APP_STATE_LOADING_H_
#define _APP_STATE_LOADING_H_

#include "AppState.h"
#include "Common.h"

class CAppStateLoading : public CAppState
{
public:
	void OnActivate();
	void OnDeactivate();

	void OnEvent(SDL_Event* Event);
	void OnUpdate();
	void OnRender();

private:
	
};

#endif