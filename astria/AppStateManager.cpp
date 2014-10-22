#include "AppStateManager.h"

//Include all app states


CAppState* CAppStateManager::ActiveAppState = 0;

void CAppStateManager::OnEvent(SDL_Event* Event)
{
	if (ActiveAppState)
		ActiveAppState->OnEvent(Event);
}

void CAppStateManager::OnLoop()
{
	if (ActiveAppState)
		ActiveAppState->OnLoop();
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
	case APPSTATE_MAIN:
		break;
	case APPSTATE_PAUSEMENU:
		break;
	}

	if (ActiveAppState) ActiveAppState->OnActivate();
}

CAppState* CAppStateManager::GetActiveAppState()
{
	return ActiveAppState;
}