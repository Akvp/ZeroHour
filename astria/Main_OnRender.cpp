#include "Main.h"

void CMain::OnRender()
{
	CAppStateManager::OnRender();
	SDL_GL_SwapWindow(Window_Main);
}