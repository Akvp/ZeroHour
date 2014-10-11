#include "Main.h"

void CMain::OnUpdate()
{
	//clamp(VerticalAngle, -3.14f / 4.0f, 3.14f / 4.0f);

	//Direction : Spherical coordinates to cartesian coordinates conversion
	Direction = glm::vec3(
		cos(VerticalAngle) * sin(HorizontalAngle),
		sin(VerticalAngle),
		cos(VerticalAngle) * cos(HorizontalAngle)
		);

	//Right vector
	Right = glm::vec3(
		sin(HorizontalAngle - 3.14f / 2.0f),
		0,
		cos(HorizontalAngle - 3.14f / 2.0f)
		);

	glm::vec3 up = glm::cross(Right, Direction);

	Projection = glm::perspective(FoV, 4.0f/3.0f, 0.1f, 1000.0f);

	//Get the view matrix using lookAt
	View = glm::lookAt(
		Position,
		Position + Direction,
		up
		);

	ModelView = View * Model;
	MVP = Projection * View * Model;
	
	//FPSControl loop to update FPS
	CFPS::FPSControl.Loop();

	std::string Text_FPS = std::string(CParams::WindowName) + "    FPS: " + std::to_string(CFPS::FPSControl.GetFPS());
	SDL_SetWindowTitle(Window_Main, Text_FPS.c_str());
}