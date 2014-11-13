#include "AppStateLoading.h"
#include "AppStateManager.h"

void CAppStateLoading::OnActivate()
{
	//Create a new thread
}

void CAppStateLoading::OnDeactivate()
{
	//Upon completion of loading, loading screen deactivation is called
	//Delete the thread
}

void CAppStateLoading::OnEvent(SDL_Event* Event)
{
	CEvent::OnEvent(Event);
}