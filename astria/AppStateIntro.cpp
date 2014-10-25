#include "AppStateManager.h"
#include "AppStateIntro.h"
#include "Params.h"
#include "Main.h"

CAppStateIntro CAppStateIntro::Instance;

CAppStateIntro::CAppStateIntro()
{

}

CAppStateIntro* CAppStateIntro::GetInstance()
{
	return &Instance;
}

void CAppStateIntro::OnActivate()
{
	StartTime = SDL_GetTicks();
	splash.Load(CMain::GetInstance()->GetRenderer(), CParams::IntroImage);
}

void CAppStateIntro::OnDeactivate()
{
	
}

void CAppStateIntro::OnEvent(SDL_Event* Event)
{
}

void CAppStateIntro::OnUpdate()
{
	if (StartTime + CParams::IntroDelay < SDL_GetTicks())
	{
		CAppStateManager::SetActiveAppState(APPSTATE_MAIN);
	}
}

void CAppStateIntro::OnRender()
{
	splash.Render(0, 0, CMain::GetInstance()->GetWindowWidth(), CMain::GetInstance()->GetWindowHeight());
	SDL_RenderPresent(CMain::GetInstance()->GetRenderer());
}

void CAppStateIntro::OnLButtonDown(int mX, int mY)
{
	CAppStateManager::SetActiveAppState(APPSTATE_MAIN);
}