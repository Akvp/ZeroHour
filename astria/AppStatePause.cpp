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
	//Color mod the snapshot to make it darker
	SDL_SetTextureBlendMode(CAppStateMain::GetInstance()->GetSnapshot(), SDL_BLENDMODE_BLEND);
	SDL_SetTextureColorMod(CAppStateMain::GetInstance()->GetSnapshot(), 100, 100, 100);

	complex.Load("ttf/complex.ttf", 25);
	complex_hover.Load("ttf/complex.ttf", 25);
	complex_hover.SetOutline(1);
	options[PAUSE_MENU_RESUME].Load(complex.GetFont(), "resume", CMain::GetInstance()->GetRenderer(), { 212, 175, 55, 1 });
	options[PAUSE_MENU_OPTIONS].Load(complex.GetFont(), "options", CMain::GetInstance()->GetRenderer(), { 212, 175, 55, 1 });
	options[PAUSE_MENU_ABOUT].Load(complex.GetFont(), "about", CMain::GetInstance()->GetRenderer(), { 212, 175, 55, 1 });
	options[PAUSE_MENU_HELP].Load(complex.GetFont(), "help", CMain::GetInstance()->GetRenderer(), { 212, 175, 55, 1 });
	options[PAUSE_MENU_EXIT].Load(complex.GetFont(), "exit", CMain::GetInstance()->GetRenderer(), { 212, 175, 55, 1 });
	options_hover[PAUSE_MENU_RESUME].Load(complex_hover.GetFont(), "resume", CMain::GetInstance()->GetRenderer(), { 212, 175, 55, 1 });
	options_hover[PAUSE_MENU_OPTIONS].Load(complex_hover.GetFont(), "options", CMain::GetInstance()->GetRenderer(), { 212, 175, 55, 1 });
	options_hover[PAUSE_MENU_ABOUT].Load(complex_hover.GetFont(), "about", CMain::GetInstance()->GetRenderer(), { 212, 175, 55, 1 });
	options_hover[PAUSE_MENU_HELP].Load(complex_hover.GetFont(), "help", CMain::GetInstance()->GetRenderer(), { 212, 175, 55, 1 });
	options_hover[PAUSE_MENU_EXIT].Load(complex_hover.GetFont(), "exit", CMain::GetInstance()->GetRenderer(), { 212, 175, 55, 1 });
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
	SDL_GetMouseState(&mouseX, &mouseY);

	for (int i = 0; i < PAUSE_MENU_COUNT; i++)
	{
		if (mouseX >= 50 &&
			mouseX <= 50 + options[i].GetWidth() &&
			mouseY >= i * 50 + 50 &&
			mouseY <= (i * 50 + 50) + options[i].GetHeight()
			)
		{
			selection = PAUSE_MENU(i);
			break;
		}
		else
		{
			selection = PAUSE_MENU_NONE;
		}
	}
}

void CAppStatePause::OnUpdate()
{
	
}

void CAppStatePause::OnRender()
{
	//Render the snapshot in the background
	SDL_Rect dest = { 0, 0, CMain::GetInstance()->GetWindowWidth(), CMain::GetInstance()->GetWindowHeight() };
	SDL_RenderCopyEx(CMain::GetInstance()->GetRenderer(), CAppStateMain::GetInstance()->GetSnapshot(), 0, &dest, 0, 0, SDL_FLIP_VERTICAL);

	//Text textures
	for (int i = 0; i < PAUSE_MENU_COUNT; i++)
	{
		if (i == selection)
		{
			options_hover[i].Render(50, i * 50 + 50);
			continue;
		}
		options[i].Render(50, i * 50 + 50);
	}

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

void CAppStatePause::OnLButtonDown(int mX, int mY)
{
	for (int i = 0; i < PAUSE_MENU_COUNT; i++)
	{
		if (mX >= 50 &&
			mX <= 50 + options[i].GetWidth() &&
			mY >= i * 50 + 50 &&
			mY <= (i * 50 + 50) + options[i].GetHeight()
			)
		{
			switch (i)
			{
			case PAUSE_MENU_RESUME:
				CAppStateManager::SetActiveAppState(APPSTATE_MAIN);
				break;
			case PAUSE_MENU_OPTIONS:
				break;
			case PAUSE_MENU_ABOUT:
				char VersionInfo[1024];
				sprintf(VersionInfo, "	Project ASTRIA	\n\n	Version: %s\n	Author: Shao Kun Deng", CParams::VersionNumber);
				MessageBox(NULL, VersionInfo, "About", MB_ICONINFORMATION);
				break;
			case PAUSE_MENU_HELP:
				break;
			case PAUSE_MENU_EXIT:
				CMain::GetInstance()->Running = false;
				break;
			}
			break;
		}
	}
}