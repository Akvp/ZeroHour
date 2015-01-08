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
		CLoadingScreen::OnActivate(&Loaded);
		OnInit_GL();
		Loaded = true;
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
	MainShader_vertex.Release();
	MainShader_fragment.Release();
	MainProgram.Release();
	Skybox.Release();
	VBO_Terrain.Release();
	glDeleteVertexArrays(1, &VAO_Terrain);
}

void CAppStateMain::OnEvent(SDL_Event* Event)
{
	if (Event->type == SDL_QUIT)
	{
		CMain::GetInstance()->Running = false;
	}

	CEvent::OnEvent(Event);

	if (Event->type == SDL_WINDOWEVENT)
	{
		if (Event->window.type == SDL_WINDOWEVENT_FOCUS_LOST)
			CAppStateManager::SetActiveAppState(APPSTATE_PAUSE);
	}

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
	glm::vec3 Up = glm::cross(Right, Direction);

	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 1000.0f);

	//Get the view matrix using lookAt
	ViewMatrix = glm::lookAt(
		Position,
		Position + Direction,
		Up
		);
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
	glBindVertexArray(VAO_Terrain);
	Texture_Terrain.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//Render models
	CModel::BindVAO();
	MainProgram.SetUniform("vEyePosition", Position);
	ModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(-10.0, 0.0, -10.0));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(3.0));
	MainProgram.SetModelAndNormalMatrix("matrices.mModel", "matrices.mNormal", ModelMatrix);
	models[0].Render();
	ModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(10.0, 0, 0));
	MainProgram.SetModelAndNormalMatrix("matrices.mModel", "matrices.mNormal", ModelMatrix);
	models[1].Render();

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

	//Load terrain
	CreateFlatGround(&VAO_Terrain, VBO_Terrain);
	Texture_Terrain.Load_2D("gfx/grass.jpg", true);
	Texture_Terrain.SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);

	//Used for wire frame
	PolyMode = GL_FILL;

	//Model matrix //Identity matrix
	ModelMatrix = glm::mat4(1.0f);

	//Load the skybox
	Skybox.Load(CParams::SkyboxFolder);

	//Load sun light
	Sun = CDirectLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(sqrt(2.0f) / 2, -sqrt(2.0f) / 2, 0), 0.2f, 1.0f);

	//Load camera properties
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
	case SDLK_ESCAPE:
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

#ifdef _DEBUG
	case SDLK_F1:	//Reserved for quick debugging
		char VersionInfo[1024];
		sprintf(VersionInfo, "Project ASTRIA\n\nVersion: %s", CParams::VersionNumber);
		MessageBox(NULL, VersionInfo, "About", MB_ICONINFORMATION);
		break;
#endif
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

void CAppStateMain::OnLoseFocus()
{
	CAppStateManager::SetActiveAppState(APPSTATE_PAUSE);
}