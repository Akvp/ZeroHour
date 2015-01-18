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
	selection = PAUSE_MENU_NONE;

	//Only load once
	if (!loaded)
	{
		//Color mod the snapshot to make it darker
		SDL_SetTextureBlendMode(CAppStateMain::GetInstance()->GetSnapshot(), SDL_BLENDMODE_BLEND);
		SDL_SetTextureColorMod(CAppStateMain::GetInstance()->GetSnapshot(), 100, 100, 100);

		//Load font
		font.Load("ttf/after_shok.ttf", 25);
		SDL_Color color = { 212, 175, 55, 1 };
		//Load all the text textures
		menu[PAUSE_MENU_RESUME].Load(font.GetFont(), "resume", CMain::GetInstance()->GetRenderer(), color);
		menu[PAUSE_MENU_OPTIONS].Load(font.GetFont(), "options", CMain::GetInstance()->GetRenderer(), color);
		menu[PAUSE_MENU_ABOUT].Load(font.GetFont(), "about", CMain::GetInstance()->GetRenderer(), color);
		menu[PAUSE_MENU_HELP].Load(font.GetFont(), "help", CMain::GetInstance()->GetRenderer(), color);
		menu[PAUSE_MENU_EXIT].Load(font.GetFont(), "exit", CMain::GetInstance()->GetRenderer(), color);

		//Load textures of text when mouse is over the text
		font.SetOutline(1);
		menu_hover[PAUSE_MENU_RESUME].Load(font.GetFont(), "resume", CMain::GetInstance()->GetRenderer(), color);
		menu_hover[PAUSE_MENU_OPTIONS].Load(font.GetFont(), "options", CMain::GetInstance()->GetRenderer(), color);
		menu_hover[PAUSE_MENU_ABOUT].Load(font.GetFont(), "about", CMain::GetInstance()->GetRenderer(), color);
		menu_hover[PAUSE_MENU_HELP].Load(font.GetFont(), "help", CMain::GetInstance()->GetRenderer(), color);
		menu_hover[PAUSE_MENU_EXIT].Load(font.GetFont(), "exit", CMain::GetInstance()->GetRenderer(), color);

		//PauseMenu = new MenuNode();
		//MenuNode* ResumeNode = new MenuNode("resume", &font, CMain::GetInstance()->GetRenderer(), color, PauseMenu);
		//PauseMenu->AddSubMenu(ResumeNode);
		//MenuNode* OptionMode = new MenuNode("options", &font, CMain::GetInstance()->GetRenderer(), color, PauseMenu);
		//PauseMenu->AddSubMenu(OptionMode);
		//MenuNode* AboutNode = new MenuNode("about", &font, CMain::GetInstance()->GetRenderer(), color, PauseMenu);
		//PauseMenu->AddSubMenu(AboutNode);
		//MenuNode* HelpNode = new MenuNode("help", &font, CMain::GetInstance()->GetRenderer(), color, PauseMenu);
		//PauseMenu->AddSubMenu(HelpNode);
		//MenuNode* ExitNode = new MenuNode("exit", &font, CMain::GetInstance()->GetRenderer(), color, PauseMenu);
		//PauseMenu->AddSubMenu(ExitNode);
		
	}
}

void CAppStatePause::OnDeactivate()
{
	
}

void CAppStatePause::OnExit()
{
	font.Release();
	for (auto op : menu)
	{
		op.Release();
	}
	for (auto op : menu_hover)
	{
		op.Release();
	}
}

void CAppStatePause::OnEvent(SDL_Event* Event)
{
	CEvent::OnEvent(Event);
	SDL_GetMouseState(&mouseX, &mouseY);

	//Only change selection when mouse is moving
	//Useful for keyboard selection
	if (Event->type == SDL_MOUSEMOTION)
	{
		//Should probably change the coordinates to a better place
		for (int i = 0; i < PAUSE_MENU_COUNT; i++)
		{
			if (mouseX >= 50 &&
				mouseX <= 50 + menu[i].GetWidth() &&
				mouseY >= i * 50 + 50 &&
				mouseY <= (i * 50 + 50) + menu[i].GetHeight()
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
		if (i == selection)
		{
			menu_hover[i].Render(50, i * 50 + 50);
			continue;
		}
		menu[i].Render(50, i * 50 + 50);
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
		if (selection == PAUSE_MENU_NONE)
		{
			selection = PAUSE_MENU_EXIT;
			break;
		}
		selection = PAUSE_MENU(selection - 1);
		if (selection < PAUSE_MENU_RESUME)
		{
			selection = PAUSE_MENU(PAUSE_MENU_COUNT + selection);
		}
		break;
	case SDLK_DOWN:
		if (selection == PAUSE_MENU_NONE)
		{
			selection = PAUSE_MENU_RESUME;
			break;
		}
		selection = PAUSE_MENU((selection + 1) % PAUSE_MENU_COUNT);
		break;
	case SDLK_RETURN:
		if (selection < PAUSE_MENU_COUNT)
		{
			OnSelect();
		}
	}
}

void CAppStatePause::OnLButtonDown(int mX, int mY)
{
	for (int i = 0; i < PAUSE_MENU_COUNT; i++)
	{
		if (mX >= 50 &&
			mX <= 50 + menu[i].GetWidth() &&
			mY >= i * 50 + 50 &&
			mY <= (i * 50 + 50) + menu[i].GetHeight()
			)
		{
			OnSelect();
			break;
		}
	}
}

void CAppStatePause::OnSelect()
{
	switch (selection)
	{
		case PAUSE_MENU_RESUME:
			CAppStateManager::SetActiveAppState(APPSTATE_MAIN);
			break;
		case PAUSE_MENU_OPTIONS:
			break;
		case PAUSE_MENU_ABOUT:
			char AboutInfo[1024];
			sprintf(AboutInfo, "\tProject ASTRIA	\n\n\tVersion: %s\n\tAuthor: Shao Kun Deng\n\tVideo card vendor: %s\n\tGPU: %s", CParams::VersionNumber, glGetString(GL_VENDOR), glGetString(GL_RENDERER));
			MessageBox(NULL, AboutInfo, "About", MB_ICONINFORMATION);
			break;
		case PAUSE_MENU_HELP:
			char HelpInfo[2048];
			sprintf(HelpInfo, "\tProject ASTRIA\t\n\n\tWASD:\tMovement\n\tQ:\tWireframe mode\t\n\tF2:\tChange movement speed\t\n\tF3:\tToggle gravity\t");
			MessageBox(NULL, HelpInfo, "Help", MB_ICONINFORMATION);
			break;
		case PAUSE_MENU_EXIT:
			CMain::GetInstance()->Running = false;
			break;
	}
}