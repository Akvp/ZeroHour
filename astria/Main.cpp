#include "Main.h"

CMain CMain::Instance;

CMain::CMain()
{
	Running = true;
	Window_Main = NULL;
	Renderer = NULL;
	Surf_Icon = NULL;
}

CMain* CMain::GetInstance()
{
	return &Instance;
}

SDL_Renderer* CMain::GetRenderer()
{
	return Renderer;
}

SDL_Window* CMain::GetWindow()
{
	return Window_Main;
}

int CMain::GetWindowWidth()
{
	return window_width;
}

int CMain::GetWindowHeight()
{
	return window_height;
}

int CMain::OnExecute()
{
	if (!OnInit())
	{
		return -1;
	}

	SDL_Event Event;

	while (Running)
	{
		while (SDL_PollEvent(&Event))
		{
			OnEvent(&Event);
		}

		OnUpdate();

		OnRender();

	}

	OnExit();

	return 0;
}

int main(int argc, char* argv[])
{
	return CMain::GetInstance()->OnExecute();
}