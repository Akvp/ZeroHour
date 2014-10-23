#include "Main.h"

void CMain::OnEvent(SDL_Event* Event)
{
	if(Event->type == SDL_QUIT)
	{
		Running = false;
	}
	CAppStateManager::OnEvent(Event);
}