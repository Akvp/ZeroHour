#include "AppStateLoading.h"

void CLoadingScreen::OnActivate(int* condition, int duration)
{
	//Create a new thread
	Thread_Loading = SDL_CreateThread(OnExecute, "LoadingThread", NULL);
}

void CLoadingScreen::OnUpdate()
{

}

void CLoadingScreen::OnRender()
{

}
