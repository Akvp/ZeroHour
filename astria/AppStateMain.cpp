#include "AppStateManager.h"
#include "AppStateMain.h"
#include "Main.h"

CAppStateMain CAppStateMain::Instance;

CAppStateMain::CAppStateMain()
{
	Loaded = false;
	snapshot = NULL;
}

CAppStateMain* CAppStateMain::GetInstance()
{
	return &Instance;
}

void CAppStateMain::OnActivate()
{
	if (!Loaded)
	{
		Speed = 0.5f;
		MouseSpeed = 0.0015f;
		OnInit_GL();

		Loaded = true;
	}

	//Hide mouse cursor
	if (SDL_ShowCursor(SDL_DISABLE) < 0)
	{
		MessageBox(NULL, SDL_GetError(), "Warning: Unable to hide cursor", MB_ICONWARNING);
	}

	//Center mouse cursor
	SDL_WarpMouseInWindow(CMain::GetInstance()->GetWindow(), CMain::GetInstance()->GetWindowWidth() / 2, CMain::GetInstance()->GetWindowHeight() / 2);

}

void CAppStateMain::OnDeactivate()
{
	//Save a snapshot of the current screen to use in pause menu
	int width = CMain::GetInstance()->GetWindowWidth();
	int height = CMain::GetInstance()->GetWindowHeight();
	SDL_Surface* Surf_Tmp = SDL_CreateRGBSurface(0, width, height, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, Surf_Tmp->pixels);
	snapshot = SDL_CreateTextureFromSurface(CMain::GetInstance()->GetRenderer(), Surf_Tmp);
	SDL_FreeSurface(Surf_Tmp);

	SDL_ShowCursor(SDL_ENABLE);
}

void CAppStateMain::OnExit()
{
	std::cout << "Releasing CAppStateMain\n";
	mainShader_vertex.release();
	mainShader_fragment.release();
	lightShader_fragment.release();
	mainProgram.release();
	skybox.release();
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
	SDL_WarpMouseInWindow(CMain::GetInstance()->GetWindow(), CMain::GetInstance()->GetWindowWidth() / 2, CMain::GetInstance()->GetWindowHeight() / 2);

	//Compute new orientation
	HorizontalAngle += MouseSpeed * float(CMain::GetInstance()->GetWindowWidth() / 2 - Mouse_X);
	VerticalAngle += MouseSpeed * float(CMain::GetInstance()->GetWindowHeight() / 2 - Mouse_Y);

}

void CAppStateMain::OnUpdate()
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
	mainProgram.SetUniform("matrices.viewMatrix", &View);
	mainProgram.SetUniform("gSampler", 0);

	mainProgram.SetUniform("matrices.modelMatrix", Model);
	mainProgram.SetUniform("matrices.normalMatrix", glm::mat4(1.0));

	//Render light
	mainProgram.SetUniform("vColor", glm::vec4(1, 1, 1, 1));
	sun.setUniform(&mainProgram, "sunLight");
	light.setUniform(&mainProgram, "pointLight");

	//Specific fragment shader (and program) for skybox so it would process light 
	//independant of other light sources in the scene
	mainProgram.interrupt();
	skyboxProgram.use();
		//Render skybox
		skyboxProgram.SetUniform("matrices.modelMatrix", glm::translate(glm::mat4(1.0), Position));
		skyboxProgram.SetUniform("matrices.viewMatrix", &View);
		skyboxProgram.SetUniform("matrices.projMatrix", &Projection);
		skyboxProgram.SetUniform("matrices.normalMatrix", glm::mat4(1.0));
		skyboxProgram.SetUniform("gSampler", 0);
		skyboxProgram.SetUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		skyboxProgram.SetUniform("fLightAmbient", sun.ambient);
		skybox.render();
	skyboxProgram.interrupt();
	mainProgram.use();

	mainProgram.SetUniform("matrices.modelMatrix", glm::mat4(1.0));
	mainProgram.SetUniform("matrices.normalMatrix", glm::mat4(1.0));
	//Render ground
	glBindVertexArray(scene_VAO);
	scene_texture.bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//Render models
	CModel::bindVAO();
	glm::mat4 mModel = glm::translate(glm::mat4(1.0), glm::vec3(-10.0, 0, -10.0));
	mModel = glm::scale(mModel, glm::vec3(3, 3, 3));
	mainProgram.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModel);
	models[0].render();
	mModel = glm::translate(glm::mat4(1.0), glm::vec3(10.0, 0, 0));
	mainProgram.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModel);
	models[1].render();

	glEnable(GL_DEPTH_TEST);

	SDL_GL_SwapWindow(CMain::GetInstance()->GetWindow());
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
	if (!ptlightShader_fragment.load("shaders/pointLight.frag", GL_FRAGMENT_SHADER))
		return false;
	if (!mainProgram.initiate(4, &mainShader_vertex, &mainShader_fragment, &lightShader_fragment, &ptlightShader_fragment))
		return false;

	skybox_fragment.load("shaders/skybox.frag", GL_FRAGMENT_SHADER);
	skyboxProgram.initiate(&mainShader_vertex, &skybox_fragment);

	//Load models
	models[0].load("gfx/Wolf/Wolf.obj");
	models[1].load("gfx/nanosuit/nanosuit.obj");
	CModel::finalizeVBO();

	//Used for wire frame
	PolyMode = GL_FILL;

	//Model matrix //Identity matrix
	Model = glm::mat4(1.0f);

	//Load the skybox
	skybox.load(CParams::SkyboxFolder);

	sun = CDirectLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(sqrt(2.0f) / 2, -sqrt(2.0f) / 2, 0), 1.0f);
	light.Position = glm::vec3(0.0f, 2.0f, 0.0f);
	light.Ambient = 0.1f;
	light.Color = glm::vec3(1.0f, 1.0f, 1.0f);

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

SDL_Texture* CAppStateMain::GetSnapshot()
{
	return snapshot;
}

void CAppStateMain::OnKeyDown(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode)
{
	switch (sym)
	{
		//Terminate the program if the user press Esc or LeftAlt+F4
	case SDLK_ESCAPE:		//TODO: Change ESC to pause and bring up a menu
		CAppStateManager::SetActiveAppState(APPSTATE_PAUSE);
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
		MoveUp = true;	//Movement bool variable is needed to counteract the inherent key repeat delay
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
		else if (PolyMode == GL_LINE)	//I think this is only useful for debugging
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