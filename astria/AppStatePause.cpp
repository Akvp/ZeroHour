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
	background.SetAlpha(100);

	complex.Load("ttf/complex.ttf", 10);
	options[0].Load(complex.GetFont(), "Some text", CMain::GetInstance()->GetRenderer());

	std::cout << "pause activate\n";
}

void CAppStatePause::OnDeactivate()
{
	complex.Release();
	for (auto op : options)
	{
		op.Release();
	}
	std::cout << "pause deactivate\n";
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
	options[0].Render(0, 0);
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