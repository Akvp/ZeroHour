#include "AppStateManager.h"
#include "AppStateIntro.h"

CAppStateIntro CAppStateIntro::Instance;

CAppStateIntro::CAppStateIntro()
{

}

void CAppStateIntro::OnActivate()
{
	StartTime = SDL_GetTicks();
}

void CAppStateIntro::OnDeactivate()
{

}

void CAppStateIntro::OnEvent(SDL_Event* Event)
{

}

void CAppStateIntro::OnLoop()
{
	if (StartTime + 3000 < SDL_GetTicks())
	{
		CAppStateManager::SetActiveAppState(APPSTATE_MAIN);
	}
}

void CAppStateIntro::OnRender()
{

}

CAppStateIntro* CAppStateIntro::GetInstance()
{
	return &Instance;
}