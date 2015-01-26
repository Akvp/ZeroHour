#ifndef _APP_STATE_LOADING_H_
#define _APP_STATE_LOADING_H_

#include <SDL/SDL_thread.h>
#include <SDL/SDL_mutex.h>

#include "Common.h"
#include "TextureBank_SDL.h"

class CLoadingScreen
{
public:	
	~CLoadingScreen();

	//Activate the loading screen
	//Loading screen will run on a seperate thread for a certain duration 
	//	or until condition variable becomes true
	//If duration is 0, loading screen will run indefinitely
	void OnActivate(int* condition, int duration = 0);

	SDL_Thread* GetThreadID();

private:
	static int OnExecute(void* ptr);

	static void OnUpdate();
	static void OnRender();

	SDL_Thread* Thread_Loading;
};

#endif