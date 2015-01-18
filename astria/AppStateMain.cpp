#include "AppStateManager.h"
#include "AppStateMain.h"
#include "Main.h"
#include "utils.h"

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
	bool loadingState = false;
	if (!Loaded)
	{
		Speed = 0.5f;
		MouseSpeed = 0.0015f;
		GravityEnabled = false;
		CLoadingScreen::OnActivate(&Loaded);
		Loaded = OnLoad();
		SDL_WaitThread(CLoadingScreen::GetThreadID(), NULL);
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
	CModel::Release();
	Particle_Test.Release();
	MainShader_vertex.Release();
	MainShader_fragment.Release();
	MainProgram.Release();
	Skybox.Release();
	CHeightMap::ReleaseShaderProgram();
	Map.Release();
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
		Clamp(FoV, 20, 80);
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
	if (MoveUp)		Position += Direction * Speed * CFPS::FPSControl.GetSpeedFactor();
	if (MoveDown)	Position -= Direction * Speed *CFPS::FPSControl.GetSpeedFactor();
	if (MoveLeft)	Position -= Right * Speed *CFPS::FPSControl.GetSpeedFactor();
	if (MoveRight)	Position += Right * Speed *CFPS::FPSControl.GetSpeedFactor();

	if (GravityEnabled)
	{
		Position.y += CParams::Gravity * CFPS::FPSControl.GetSpeedFactor();
		Position.y = max(Position.y, Map.GetHeight(Position) + 5);
	}

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

	//Up vector
	Up = glm::cross(Right, Direction);

	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 1000.0f);

	//Get the view matrix using lookAt
	ViewMatrix = glm::lookAt(
		Position,
		Position + Direction,
		Up
		);

	//Set matrices for particle program
	Particle_Test.SetMatrices(&ProjectionMatrix, &ViewMatrix, Direction);
}

void CAppStateMain::OnRender()
{
	//Clear the screen for each frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use shaders
	MainProgram.Use();

	MainProgram.SetUniform("matrices.mProjection", &ProjectionMatrix);
	MainProgram.SetUniform("matrices.mView", &ViewMatrix);
	MainProgram.SetUniform("mat.diffuse", 0);
	MainProgram.SetUniform("mat.specular", 2);

	MainProgram.SetUniform("matrices.mModel", ModelMatrix);
	MainProgram.SetUniform("matrices.mNormal", glm::mat4(1.0));

	//Render light
	Sun.SetUniform(&MainProgram, "sunLight");

	//Render Skybox
	MainProgram.SetUniform("matrices.mModel", glm::translate(glm::mat4(1.0), Position));
	MainProgram.SetUniform("bSkybox", 1);
	Skybox.Render();
	MainProgram.SetUniform("bSkybox", 0);

	//Reset model matrix
	MainProgram.SetUniform("matrices.mModel", glm::mat4(1.0));

	//Render ground
	CShaderProgram* Program_Terrain = CHeightMap::GetShaderProgram();
	Program_Terrain->Use();
	Program_Terrain->SetUniform("matrices.projMatrix", ProjectionMatrix);
	Program_Terrain->SetUniform("matrices.viewMatrix", ViewMatrix);
	Program_Terrain->SetUniform("vEyePosition", Position);
	Program_Terrain->SetUniform("LowAlt.diffuse", 0);	Program_Terrain->SetUniform("LowAlt.specular", 1);
	Program_Terrain->SetUniform("MidAlt.diffuse", 2);	Program_Terrain->SetUniform("MidAlt.specular", 3);
	Program_Terrain->SetUniform("HighAlt.diffuse", 4);	Program_Terrain->SetUniform("HighAlt.specular", 5);
	Program_Terrain->SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", glm::mat4(1.0));
	Program_Terrain->SetUniform("vColor", glm::vec4(1, 1, 1, 1));
	Sun.SetUniform(Program_Terrain, "sunLight");
	//Diffuse textures			//Specular textures
	Texture_Terrain[0].Bind(0);	Texture_Terrain[1].Bind(1);
	Texture_Terrain[2].Bind(2); Texture_Terrain[3].Bind(3);
	Texture_Terrain[4].Bind(4);	Texture_Terrain[5].Bind(5);
	Map.Render();

	MainProgram.Use();
	//Render models
	CModel::BindVAO();
	MainProgram.SetUniform("vEyePosition", Position);
	ModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(88, 30, 176));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0));
	MainProgram.SetModelAndNormalMatrix("matrices.mModel", "matrices.mNormal", ModelMatrix);
	models[0].Render();
	ModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(83, 30, 180));
	MainProgram.SetModelAndNormalMatrix("matrices.mModel", "matrices.mNormal", ModelMatrix);
	models[1].Render();

	//Render particles
	Texture_Particle.Bind();
	float fps = CFPS::FPSControl.GetFPS();
	float FrameInterval = 1 / fps;
	Particle_Test.Update(FrameInterval);
	Particle_Test.Render();

	SDL_GL_SwapWindow(CMain::GetInstance()->GetWindow());
}

bool CAppStateMain::OnLoad()
{
	//Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum glewInitStatus = glewInit();
	if (glewInitStatus != GLEW_OK)
	{
		Error("Loading error", (char*)(glewGetErrorString(glewInitStatus)));
		return false;
	}

	//Disable Vsync to prevent framerate capping at refresh rate
	if (SDL_GL_SetSwapInterval(0) < 0)
	{
		Warning("Loading error", "Warning: Unable to set VSync\n" + string(SDL_GetError()));
	}

	//Clear the background as dark blue
	glClearColor(0.1f, 0.1f, 0.4f, 0.0f);

	//Enable depth test
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);

	//Remove triangles which normal is not towards the camera (do not render the inside of the model)
	//glEnable(GL_CULL_FACE);

	//Load shaders and programs
	if (!MainShader_vertex.Load("shaders/main_shader.vert", GL_VERTEX_SHADER))
		return false;
	if (!MainShader_fragment.Load("shaders/main_shader.frag", GL_FRAGMENT_SHADER))
		return false;
	if (!MainProgram.Initiate(&MainShader_vertex, &MainShader_fragment))
		return false;

	//Load models
	models[0].Load("gfx/Wolf/Wolf.obj");
	models[1].Load("gfx/nanosuit/nanosuit.obj");
	CModel::UploadVBO();

	//Load particles
	Texture_Particle.Load_2D("gfx/particle.bmp", true);
	Particle_Test.Init();
	Particle_Test.Set(
		glm::vec3(-138, 85, 165),	//Position
		glm::vec3(-10, 2, -10),	//Minimum velocity
		glm::vec3(10, 20, 10),	//Maximum velocity
		glm::vec3(0, -5, 0),	//Gravity
		glm::vec3(0.8, 0.2, 0.2),	//Color
		1.5f,	//Minimum lifespan in second
		3.0f,	//Maximum lifespan in second
		0.25f, 	//Size
		0.02,	//Spawn interval
		100);	//Count i.e. number generated per frame

	//Load terrain
	string TextureNames[] = { "sand.jpg", "sand_specular.jpg", "grass.jpg", "grass_specular.jpg", "snow.jpg", "snow_specular.png"};
	for (int i = 0; i < 6; i++)
	{
		Texture_Terrain[i].Load_2D("gfx/" + TextureNames[i], true);
		Texture_Terrain[i].SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
	}
	Map.Load("gfx/3.jpg");
	CHeightMap::LoadShaderProgram();
	Map.SetRenderSize(CParams::WorldX, CParams::WorldY, CParams::WorldZ);

	//Used for wire frame
	PolyMode = GL_FILL;

	//Model matrix //Identity matrix
	ModelMatrix = glm::mat4(1.0f);

	//Load the skybox
	Skybox.Load(CParams::SkyboxFolder);

	//Load sun light
	Sun = CDirectLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1, -3, 0), 0.2f, 1.0f);

	//Load camera properties
	Position = glm::vec3(84, 34, 210);
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
	case SDLK_ESCAPE:
		CAppStateManager::SetActiveAppState(APPSTATE_PAUSE);
		break;
	case SDLK_F4:
		if (scancode == SDL_SCANCODE_LALT)
		{
			CMain::GetInstance()->Running = false;
		}
		break;
	case SDLK_F2:
		//Faster movement speed for quicker debugging xD
		Speed = (Speed == 0.5) ? 5 : 0.5;
		break;
	case SDLK_F3:
		//Disable gravity
		GravityEnabled = !GravityEnabled;
		break;

		//Movement Keys
	case SDLK_w:
		MoveUp = true;	//Movement bool variable is needed to counteract the inherent key repeat delay
		break;
	case SDLK_s:
		MoveDown = true;
		break;
	case SDLK_d:
		MoveRight = true;
		break;
	case SDLK_a:
		MoveLeft = true;
		break;
	case SDLK_UP:
		Sun.Ambient += 0.1;
		break;
	case SDLK_DOWN:
		Sun.Ambient -= 0.1;
		break;

		//Switch between normal model and wireframe
	case SDLK_q:
		if (PolyMode == GL_FILL)
		{
			PolyMode = GL_LINE;
			glPolygonMode(GL_FRONT_AND_BACK, PolyMode);
		}
		else if (PolyMode == GL_LINE)	//Useless but pretty cool
		{
			PolyMode = GL_FILL;
			glPolygonMode(GL_FRONT_AND_BACK, PolyMode);
		}
		break;
	}
}

void CAppStateMain::OnKeyUp(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode)
{
	switch (sym)
	{
	case SDLK_w:
		MoveUp = false;
		break;
	case SDLK_s:
		MoveDown = false;
		break;
	case SDLK_d:
		MoveRight = false;
		break;
	case SDLK_a:
		MoveLeft = false;
		break;
	}
}

void CAppStateMain::OnLoseFocus()
{
	CAppStateManager::SetActiveAppState(APPSTATE_PAUSE);
}