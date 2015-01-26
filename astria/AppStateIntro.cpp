#include "AppStateManager.h"
#include "AppStateIntro.h"
#include "Params.h"
#include "Main.h"
#include "FileManager.h"

CAppStateIntro CAppStateIntro::Instance;
std::string HelpText;
std::string CreditsText;

CAppStateIntro* CAppStateIntro::GetInstance()
{
	return &Instance;
}

void CAppStateIntro::OnActivate()
{
	Selection = INTRO_MENU_NONE;
	splash.Load(CMain::GetInstance()->GetRenderer(), CParams::IntroImage);
	OffsetX = 50;
	OffsetY = 500;
	HelpText = "Error loading help string\n";
	CreditsText = "Error loading credits file\n";
	HelpText = FileManager::GetContents("help.txt");
	CreditsText = FileManager::GetContents("credits.txt");
	LoadFontTextures();
	Font.Release();
}

void CAppStateIntro::OnDeactivate()
{
	splash.Release();
	for (auto el : Menu)		el.Release();
	for (auto el : MenuHover)	el.Release();
}

void CAppStateIntro::OnEvent(SDL_Event* Event)
{
	CEvent::OnEvent(Event);
	SDL_GetMouseState(&MouseX, &MouseY);
	if (Event->type == SDL_MOUSEMOTION)
	{
		for (int i = 0; i < PAUSE_MENU_COUNT; i++)
		{
			if (MouseX >= OffsetX &&
				MouseX <= OffsetX + Menu[i].GetWidth() &&
				MouseY >= i * 50 + OffsetY &&
				MouseY <= (i * 50 + OffsetY) + Menu[i].GetHeight()
				)
			{
				Selection = INTRO_MENU(i);
				break;
			}
			else
			{
				Selection = INTRO_MENU_NONE;
			}
		}
	}
}

void CAppStateIntro::OnUpdate()
{
	
}

void CAppStateIntro::OnRender()
{
	splash.Render(0, 0, CMain::GetInstance()->GetWindowWidth(), CMain::GetInstance()->GetWindowHeight());

	//Render text textures
	for (int i = 0; i < INTRO_MENU_COUNT; i++)
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

void CAppStateIntro::LoadFontTextures()
{
	//Load font
	Font.Load("ttf/after_shok.ttf", 25);
	SDL_Color Color = { 204, 204, 204, 1 };
	SDL_Renderer* MainRenderer = CMain::GetInstance()->GetRenderer();

	std::string MenuText[] = { "start", "about", "help", "credits", "exit" };

	for (int i = 0; i < INTRO_MENU_COUNT; i++)	Menu[i].Load(&Font, MenuText[i], MainRenderer, Color);
	Font.SetOutline(1);
	for (int i = 0; i < INTRO_MENU_COUNT; i++)	MenuHover[i].Load(&Font, MenuText[i], MainRenderer, Color);
}

void CAppStateIntro::OnLButtonDown(int mX, int mY)
{
	if (Selection != INTRO_MENU_NONE)
		OnSelect();
}

void CAppStateIntro::OnSelect()
{
	switch (Selection)
	{
	case INTRO_MENU_START:
		CAppStateManager::SetActiveAppState(APPSTATE_MAIN);
		break;
	case INTRO_MENU_ABOUT:
		char AboutInfo[1024];
		sprintf(AboutInfo, "\tProject ASTRIA	\n\n\tVersion: %s\n\tAuthor: Shao Kun Deng\n\tVideo card vendor: %s\n\tGPU: %s", CParams::VersionNumber, glGetString(GL_VENDOR), glGetString(GL_RENDERER));
		MessageBox(NULL, AboutInfo, "About", MB_ICONINFORMATION);
		break;
	case INTRO_MENU_HELP:
		MessageBox(NULL, HelpText.c_str(), "Help", MB_ICONINFORMATION);
		break;
	case INTRO_MENU_CREDITS:
		MessageBox(NULL, CreditsText.c_str(), "Credits", MB_ICONINFORMATION);
		break;
	case INTRO_MENU_EXIT:
		CMain::GetInstance()->Running = false;
		break;
	}
}