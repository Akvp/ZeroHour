#include "AppStateManager.h"
#include "AppStateMain.h"
#include "Main.h"

CAppStateMain CAppStateMain::Instance;

CAppStateMain::CAppStateMain()
{

}

CAppStateMain* CAppStateMain::GetInstance()
{
	return &Instance;
}

void CAppStateMain::OnActivate()
{
	Speed = 0.5f;
	MouseSpeed = 0.002f;
	OnInit_GL();

	//Hide mouse cursor
	if (SDL_ShowCursor(SDL_DISABLE) < 0)
	{
		MessageBox(NULL, SDL_GetError(), "Warning: Unable to hide cursor", MB_ICONWARNING);
	}

	//Center mouse cursor
	SDL_WarpMouseInWindow(CMain::GetInstance()->GetWindow(), CParams::WindowWidth / 2, CParams::WindowHeight / 2);
}

void CAppStateMain::OnDeactivate()
{
	mainShader_vertex.release();
	mainShader_fragment.release();
	lightShader_fragment.release();
	mainProgram.release();
	skybox.release();

	SDL_ShowCursor(SDL_ENABLE);
}

void CAppStateMain::OnEvent(SDL_Event* Event)
{
	if (Event->type == SDL_QUIT)
	{
		CMain::GetInstance()->Running = false;
	}

	CEvent::OnEvent(Event);

	if (Event->type == SDL_MOUSEWHEEL)
	{
		FoV -= 5 * Event->wheel.y;
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
	SDL_WarpMouseInWindow(CMain::GetInstance()->GetWindow(), CParams::WindowWidth / 2, CParams::WindowHeight / 2);

	//Compute new orientation
	HorizontalAngle += MouseSpeed * float(CParams::WindowWidth / 2 - Mouse_X);
	VerticalAngle += MouseSpeed * float(CParams::WindowHeight / 2 - Mouse_Y);

}

void CAppStateMain::OnLoop()
{
	//clamp(VerticalAngle, -3.14f / 4.0f, 3.14f / 4.0f);

	if (MoveUp)		Position += Direction * Speed * CFPS::FPSControl.GetSpeedFactor();
	if (MoveDown)	Position -= Direction * Speed *CFPS::FPSControl.GetSpeedFactor();
	if (MoveLeft)	Position -= Right * Speed *CFPS::FPSControl.GetSpeedFactor();
	if (MoveRight)	Position += Right * Speed *CFPS::FPSControl.GetSpeedFactor();

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

	Projection = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 1000.0f);

	//Get the view matrix using lookAt
	View = glm::lookAt(
		Position,
		Position + Direction,
		up
		);

	ModelView = View * Model;
	MVP = Projection * View * Model;
}

void CAppStateMain::OnRender()
{
	//Clear the screen for each frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use shaders
	mainProgram.use();

	mainProgram.SetUniform("matrices.projMatrix", &Projection);
	mainProgram.SetUniform("matrices.viewMatrix", View);
	mainProgram.SetUniform("gSampler", 0);

	mainProgram.SetUniform("matrices.modelMatrix", Model);
	mainProgram.SetUniform("matrices.normalMatrix", glm::mat4(1.0));

	//Render directional sun light
	mainProgram.SetUniform("vColor", glm::vec4(1, 1, 1, 1));
	sun.setUniform(&mainProgram, "sunLight");

	//Render skybox
	mainProgram.SetUniform("matrices.modelMatrix", glm::translate(glm::mat4(1.0), Position));
	skybox.render();

	mainProgram.SetUniform("matrices.modelMatrix", glm::mat4(1.0));

	//Render ground
	glBindVertexArray(scene_VAO);
	scene_texture.bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//Render models
	CModel::bindVAO();
	float xPosHouse = -10.0f;
	glm::mat4 mModel = glm::translate(glm::mat4(1.0), glm::vec3(xPosHouse, 0, 0));
	mModel = glm::scale(mModel, glm::vec3(3, 3, 3));
	mainProgram.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModel);
	models[0].render();
	//models[1].render();

	glEnable(GL_DEPTH_TEST);
}

bool CAppStateMain::OnInit_GL()
{
	//Clear the background as dark blue
	glClearColor(0.1f, 0.1f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);

	//Remove triangles which normal is not towards the camera (do not render the inside of the model)
	//glEnable(GL_CULL_FACE);


	//Load shaders and programs
	if (!mainShader_vertex.load("shaders/main_shader.vert", GL_VERTEX_SHADER))
		return false;
	if (!mainShader_fragment.load("shaders/main_shader.frag", GL_FRAGMENT_SHADER))
		return false;
	if (!lightShader_fragment.load("shaders/dirLight.frag", GL_FRAGMENT_SHADER))
		return false;
	if (!mainProgram.initiate(3, &mainShader_vertex, &mainShader_fragment, &lightShader_fragment))
		return false;

	//Load models
	models[0].load("gfx/Wolf/Wolf.obj");
	//models[1].load("gfx/audi_rsq/audi_rsq.obj");
	CModel::finalizeVBO();

	//Used for wire frame
	PolyMode = GL_FILL;

	//Model matrix //Identity matrix
	Model = glm::mat4(1.0f);

	//Load the skybox
	skybox.load(CParams::SkyboxFolder,
		CParams::SkyboxFront,
		CParams::SkyboxBack,
		CParams::SkyboxLeft,
		CParams::SkyboxRight,
		CParams::SkyboxTop,
		CParams::SkyboxBot);

	sun = CDirectLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(sqrt(2.0f) / 2, -sqrt(2.0f) / 2, 0), 1.0f);

	CreateStaticSceneObjects(&scene_VAO, scene_VBO);
	scene_texture.load_2D("gfx/sand_grass_02.jpg", true);
	scene_texture.setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
	test_texture.load_2D("gfx/metal.jpg", true);
	test_texture.setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);

	Position = glm::vec3(30, 5, 30);
	FoV = 45.0f;
	HorizontalAngle = -3.14f;
	VerticalAngle = 0.0f;


	return true;
}

void CAppStateMain::OnKeyDown(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode)
{
	std::string pos = std::to_string(Position.x) + ' ' + std::to_string(Position.y) + ' ' + std::to_string(Position.z);
	std::string angle = std::to_string(HorizontalAngle) + ' ' + std::to_string(VerticalAngle);

	switch (sym)
	{
		//Terminate the program if the user press Esc or LeftAlt+F4
	case SDLK_ESCAPE:		//TODO: Change ESC to pause and bring up a menu
		CMain::GetInstance()->Running = false;
		break;
	case SDLK_F4:
		if (scancode == SDL_SCANCODE_LALT)
		{
			CMain::GetInstance()->Running = false;
		}
		break;

		//Movement Keys
	case SDLK_w:
	case SDLK_UP:
		MoveUp = true;
		break;
	case SDLK_s:
	case SDLK_DOWN:
		MoveDown = true;
		break;
	case SDLK_d:
	case SDLK_RIGHT:
		MoveRight = true;
		break;
	case SDLK_a:
	case SDLK_LEFT:
		MoveLeft = true;
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

void CAppStateMain::OnKeyUp(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode)
{
	switch (sym)
	{
	case SDLK_w:
	case SDLK_UP:
		MoveUp = false;
		break;
	case SDLK_s:
	case SDLK_DOWN:
		MoveDown = false;
		break;
	case SDLK_d:
	case SDLK_RIGHT:
		MoveRight = false;
		break;
	case SDLK_a:
	case SDLK_LEFT:
		MoveLeft = false;
		break;
	}
}