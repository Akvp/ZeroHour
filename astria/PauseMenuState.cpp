#include "PauseMenuState.h"
#include "AppStateManager.h"

void COptions::OnActivate()
{
	
}

void COptions::OnDeactivate()
{

}

void COptions::OnEvent(SDL_Event* Event)
{
	CEvent::OnEvent(Event);
}

void COptions::OnUpdate()
{

}

void COptions::OnRender()
{
	//Render the snapshot in the background
	SDL_Rect dest = { 0, 0, CMain::GetInstance()->GetWindowWidth(), CMain::GetInstance()->GetWindowHeight() };
	SDL_RenderCopyEx(CMain::GetInstance()->GetRenderer(), CAppStateMain::GetInstance()->GetSnapshot(), 0, &dest, 0, 0, SDL_FLIP_VERTICAL);

}