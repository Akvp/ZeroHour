#include "Main.h"

void CMain::OnExit()
{
	CAppStateManager::SetActiveAppState(APPSTATE_NONE);

	SDL_DestroyWindow(Window_Main);
	Window_Main = NULL;

	SDL_GL_DeleteContext(glContext);

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}