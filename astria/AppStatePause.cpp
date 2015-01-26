#include "AppStatePause.h"
#include "AppStateManager.h"

CAppStatePause CAppStatePause::Instance;

CAppStatePause::CAppStatePause()
{
	loaded = false;
}

CAppStatePause* CAppStatePause::GetInstance()
{
	return &Instance;
}

void CAppStatePause::OnActivate()
{
	Selection = PAUSE_MENU_NONE;

	OffsetX = 50; OffsetY = 50;
	//Only load once
	if (!loaded)
	{
		//Color mod the snapshot to make it darker
		SDL_SetTextureBlendMode(CAppStateMain::GetInstance()->GetSnapshot(), SDL_BLENDMODE_BLEND);
		SDL_SetTextureColorMod(CAppStateMain::GetInstance()->GetSnapshot(), 100, 100, 100);

		//Load Font
		Font.Load("ttf/after_shok.ttf", 25);
		SDL_Color Color = { 204, 204, 204, 1 };

		SDL_Renderer* MainRenderer = CMain::GetInstance()->GetRenderer();

		std::string MenuText[] = { "resume", "credits", "about", "help", "exit" };

		//Load all the text textures
		for (int i = 0; i < PAUSE_MENU_COUNT; i++)	Menu[i].Load(&Font, MenuText[i], MainRenderer, Color);

		//Load textures of text when mouse is over the text
		Font.SetOutline(1);
		for (int i = 0; i < PAUSE_MENU_COUNT; i++)	MenuHover[i].Load(&Font, MenuText[i], MainRenderer, Color);


	}
}

void CAppStatePause::OnDeactivate()
{
	
}

void CAppStatePause::OnExit()
{
	Font.Release();
	for (auto op : Menu)		op.Release();
	for (auto op : MenuHover)	op.Release();
}

void CAppStatePause::OnEvent(SDL_Event* Event)
{
	CEvent::OnEvent(Event);
	SDL_GetMouseState(&MouseX, &MouseY);

	//Only change Selection when mouse is moving
	//Useful for keyboard Selection
	if (Event->type == SDL_MOUSEMOTION)
	{
		//Should probably change the coordinates to a better place
		for (int i = 0; i < PAUSE_MENU_COUNT; i++)
		{
			if (MouseX >= OffsetX &&
				MouseX <= OffsetX + Menu[i].GetWidth() &&
				MouseY >= i * 50 + OffsetY &&
				MouseY <= (i * 50 + OffsetY) + Menu[i].GetHeight()
				)
			{
				Selection = PAUSE_MENU(i);
				break;
			}
			else
			{
				Selection = PAUSE_MENU_NONE;
			}
		}
	}

}

void CAppStatePause::OnUpdate()
{
	//No updates
	//How boring
}

void CAppStatePause::OnRender()
{
	//Render the snapshot in the background
	SDL_Rect dest = { 0, 0, CMain::GetInstance()->GetWindowWidth(), CMain::GetInstance()->GetWindowHeight() };
	SDL_RenderCopyEx(CMain::GetInstance()->GetRenderer(), CAppStateMain::GetInstance()->GetSnapshot(), 0, &dest, 0, 0, SDL_FLIP_VERTICAL);

	//Text textures
	for (int i = 0; i < PAUSE_MENU_COUNT; i++)
	{
		if (i == Selection)
		{
			MenuHover[i].Render(OffsetX, i * 50 + OffsetY);
			continue;
		}
		Menu[i].Render(OffsetX, i * 50 + OffsetY);
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
	case SDLK_UP:
		if (Selection == PAUSE_MENU_NONE)
		{
			Selection = PAUSE_MENU_EXIT;
			break;
		}
		Selection = PAUSE_MENU(Selection - 1);
		if (Selection < PAUSE_MENU_RESUME)
		{
			Selection = PAUSE_MENU(PAUSE_MENU_COUNT + Selection);
		}
		break;
	case SDLK_DOWN:
		if (Selection == PAUSE_MENU_NONE)
		{
			Selection = PAUSE_MENU_RESUME;
			break;
		}
		Selection = PAUSE_MENU((Selection + 1) % PAUSE_MENU_COUNT);
		break;
	case SDLK_RETURN:
		if (Selection < PAUSE_MENU_COUNT)
		{
			OnSelect();
		}
	}
}

void CAppStatePause::OnLButtonDown(int mX, int mY)
{
	if (Selection != PAUSE_MENU_NONE)
		OnSelect();
}

void CAppStatePause::OnSelect()
{
	switch (Selection)
	{
		case PAUSE_MENU_RESUME:
			CAppStateManager::SetActiveAppState(APPSTATE_MAIN);
			break;
		case PAUSE_MENU_CREDITS:
			MessageBox(NULL, CreditsText.c_str(), "Credits", MB_ICONINFORMATION);
			break;
		case PAUSE_MENU_ABOUT:
			char AboutInfo[1024];
			sprintf(AboutInfo, "\tProject ASTRIA	\n\n\tVersion: %s\n\tAuthor: Shao Kun Deng\n\tVideo card vendor: %s\n\tGPU: %s", CParams::VersionNumber, glGetString(GL_VENDOR), glGetString(GL_RENDERER));
			MessageBox(NULL, AboutInfo, "About", MB_ICONINFORMATION);
			break;
		case PAUSE_MENU_HELP:
			MessageBox(NULL, HelpText.c_str(), "Help", MB_ICONINFORMATION);
			break;
		case PAUSE_MENU_EXIT:
			CMain::GetInstance()->Running = false;
			break;
	}
}