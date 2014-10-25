#include "AppStateManager.h"

CAppState* CAppStateManager::ActiveAppState = 0;

void CAppStateManager::OnEvent(SDL_Event* Event)
{
	if (ActiveAppState)
		ActiveAppState->OnEvent(Event);
}

void CAppStateManager::OnUpdate()
{
	if (ActiveAppState)
		ActiveAppState->OnUpdate();
}

void CAppStateManager::OnRender()
{
	if (ActiveAppState)
		ActiveAppState->OnRender();
}

void CAppStateManager::SetActiveAppState(int AppStateID)
{
	if (ActiveAppState)
		ActiveAppState->OnDeactivate();

	//All the app state switches
	switch (AppStateID)
	{
	case APPSTATE_NONE:
		ActiveAppState = 0;
		break;
	case APPSTATE_INTRO:
		ActiveAppState = CAppStateIntro::GetInstance();
		break;
	case APPSTATE_MAIN:
		ActiveAppState = CAppStateMain::GetInstance();
		break;
	case APPSTATE_PAUSE:
		ActiveAppState = CAppStatePause::GetInstance();
		break;
	}

	if (ActiveAppState) ActiveAppState->OnActivate();
}

CAppState* CAppStateManager::GetActiveAppState()
{
	return ActiveAppState;
}