#include "Main.h"

void CMain::OnEvent(SDL_Event Event)
{
	if(Event.type == SDL_QUIT)
	{
		Running = false;
	}

	CEvent::OnEvent(&Event);

	if (Event.type == SDL_MOUSEWHEEL)
	{
		FoV -= 5 * Event.wheel.y;
		if (FoV > 80)
		{
			FoV = 80;
		}
		if (FoV < 20)
		{
			FoV = 20;
		}
	}

	//Get mouse position
	SDL_GetMouseState(&Mouse_X, &Mouse_Y);

	//Recenter mouse
	SDL_WarpMouseInWindow(Window_Main, CParams::WindowWidth / 2, CParams::WindowHeight / 2);

	//Compute new orientation
	HorizontalAngle += MouseSpeed * float(CParams::WindowWidth / 2 - Mouse_X);
	VerticalAngle += MouseSpeed * float(CParams::WindowHeight / 2 - Mouse_Y);
}

void CMain::OnKeyDown(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode)
{
	std::string pos = std::to_string(Position.x) + ' ' + std::to_string(Position.y) + ' ' + std::to_string(Position.z);
	std::string angle = std::to_string(HorizontalAngle) + ' ' + std::to_string(VerticalAngle);

	switch (sym)
	{
	//Terminate the program if the user press Esc or LeftAlt+F4
	case SDLK_ESCAPE:		//TODO: Change ESC to pause and bring up a menu
		Running = false;	
		break;
	case SDLK_F4:
		if (scancode == SDL_SCANCODE_LALT)
		{
			Running = false;
		}
		break;

	//Movement Keys
	case SDLK_w:
	case SDLK_UP:
		Position += Direction * Speed * CFPS::FPSControl.GetSpeedFactor();
		break;
	case SDLK_s:
	case SDLK_DOWN:
		Position -= Direction * Speed *CFPS::FPSControl.GetSpeedFactor();
		break;
	case SDLK_d:
	case SDLK_RIGHT:
		Position += Right * Speed *CFPS::FPSControl.GetSpeedFactor();
		break;
	case SDLK_a:
	case SDLK_LEFT:
		Position -= Right * Speed *CFPS::FPSControl.GetSpeedFactor();
		break;

	//Switch between normal model and wireframe
	case SDLK_q:
		if (PolyMode == GL_FILL)
		{
			PolyMode = GL_LINE;
			glPolygonMode(GL_FRONT_AND_BACK, PolyMode);
		}
		else if (PolyMode == GL_LINE)
		{
			PolyMode = GL_FILL;
			glPolygonMode(GL_FRONT_AND_BACK, PolyMode);
		}
		break;

	case SDLK_F1:
		char VersionInfo[1024];
		sprintf(VersionInfo, "Project ASTRIA\n\nVersion: %s", CParams::VersionNumber);
		MessageBox(NULL, VersionInfo, "About", MB_ICONINFORMATION);
		break;
	case SDLK_F2:
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Angles", angle.c_str(), NULL);
		break;
	}
}
