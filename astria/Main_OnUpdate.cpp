#include "hMain.h"

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

	Projection = glm::perspective(FoV, WINDOW_RATIO, 0.1f, 1000.0f);

	View = glm::lookAt(
		Position,
		Position + Direction,
		up
		);

	ModelView = View * Model;
	ModelView3x3 = glm::mat3(ModelView);
	MVP = Projection * View * Model;
	
	CFPS::FPSControl.Loop();

	std::string Text_FPS = "OpenGL_Test   FPS: " + std::to_string(CFPS::FPSControl.GetFPS());
	SDL_SetWindowTitle(Window_Main, Text_FPS.c_str());
}