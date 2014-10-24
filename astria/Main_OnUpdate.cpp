#include "Main.h"

void CMain::OnUpdate()
{
	CAppStateManager::OnUpdate();

	//FPSControl loop to update FPS
	CFPS::FPSControl.Loop();

	//Reset window title to add current FPS
	std::string Text_FPS = std::string(CParams::WindowName) + "    FPS: " + std::to_string(CFPS::FPSControl.GetFPS());
	SDL_SetWindowTitle(Window_Main, Text_FPS.c_str());
}