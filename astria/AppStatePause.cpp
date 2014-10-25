#include "AppStatePause.h"
#include "AppStateManager.h"

CAppStatePause CAppStatePause::Instance;

CAppStatePause::CAppStatePause()
{
	
}

CAppStatePause* CAppStatePause::GetInstance()
{
	return &Instance;
}

void CAppStatePause::OnActivate()
{
	selection = PAUSE_MENU_NONE;
	background.Load(CMain::GetInstance()->GetRenderer(), "gfx/img/bt.jpg");
	background.SetBlend(SDL_BLENDMODE_BLEND);
	background.SetAlpha(1);

	complex.Load("ttf/complex.ttf", 20);
	options[0].Load(complex.GetFont(), "some text", CMain::GetInstance()->GetRenderer());
}

void CAppStatePause::OnDeactivate()
{
	complex.Release();
	for (auto op : options)
	{
		op.Release();
	}
}

void CAppStatePause::OnEvent(SDL_Event* Event)
{
	CEvent::OnEvent(Event);
}

void CAppStatePause::OnUpdate()
{
	
}

void CAppStatePause::OnRender()
{
	background.Render(0, 0, CMain::GetInstance()->GetWindowWidth(), CMain::GetInstance()->GetWindowHeight());
	options[0].Render(50, 100);
	SDL_RenderPresent(CMain::GetInstance()->GetRenderer());
}

void CAppStatePause::OnKeyDown(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode)
{
	switch (sym)
	{
	case SDLK_ESCAPE:
		CAppStateManager::SetActiveAppState(APPSTATE_MAIN);
		break;
	}
}