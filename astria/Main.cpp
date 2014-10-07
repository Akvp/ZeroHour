#include "Main.h"

CMain::CMain()
{
	Running = true;
	Window_Main = NULL;

	Speed = 5.0f;
	MouseSpeed = 0.002f;
}

CMain* CMain::GetInstance()
{
	return &Instance;
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
			OnEvent(Event);
		}

		OnUpdate();

		OnRender();

	}

	OnExit();

	return 0;
}

int main(int argc, char* argv[])
{
	CMain::GetInstance()->OnExecute();
	return 0;
}