#include "Main.h"

void CMain::OnUpdate()
{
	CAppStateManager::OnLoop();

	//FPSControl loop to update FPS
	CFPS::FPSControl.Loop();

	std::string Text_FPS = std::string(CParams::WindowName) + "    FPS: " + std::to_string(CFPS::FPSControl.GetFPS());
	SDL_SetWindowTitle(Window_Main, Text_FPS.c_str());
}