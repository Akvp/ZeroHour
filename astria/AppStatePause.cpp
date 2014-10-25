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
	CAppStateMain::GetInstance()->GetSnapshot();
	SDL_SetTextureBlendMode(CAppStateMain::GetInstance()->GetSnapshot(), SDL_BLENDMODE_BLEND);
	SDL_SetTextureColorMod(CAppStateMain::GetInstance()->GetSnapshot(), 100, 100, 100);

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
	SDL_Rect dest = { 0, 0, CMain::GetInstance()->GetWindowWidth(), CMain::GetInstance()->GetWindowHeight() };
	SDL_RenderCopyEx(CMain::GetInstance()->GetRenderer(), CAppStateMain::GetInstance()->GetSnapshot(), 0, &dest, 0, 0, SDL_FLIP_VERTICAL);
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