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
	splash.Load(CMain::GetInstance()->GetRenderer(), "gfx/img/splash.jpg");
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
	splash.Render(0, 0);// , CParams::WindowWidth, CParams::WindowHeight);
}

void CAppStateIntro::OnLButtonDown(int mX, int mY)
{
	CAppStateManager::SetActiveAppState(APPSTATE_MAIN);
}