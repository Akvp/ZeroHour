#include "Main.h"

void CMain::OnExit()
{
	CAppStateManager::SetActiveAppState(APPSTATE_NONE);

	CAppStateMain::GetInstance()->OnExit();
	CAppStatePause::GetInstance()->OnExit();

	SDL_DestroyWindow(Window_Main);
	Window_Main = NULL;
	SDL_FreeSurface(Surf_Icon);
	Surf_Icon = NULL;
	SDL_DestroyRenderer(Renderer);
	Renderer = NULL;
	SDL_GL_DeleteContext(glContext);

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}