#include "AppStateManager.h"
#include "AppStateMain.h"
#include "Main.h"
#include "utils.h"
#include "Stringify.h"

CAppStateMain CAppStateMain::Instance;

CAppStateMain::CAppStateMain()
{
	Loaded = 0;
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
		FogEnabled = 1;
		CLoadingScreen::OnActivate(&Loaded);
		Loaded = OnLoad();
		if (!Loaded)	CMain::GetInstance()->Running = false;
		SDL_WaitThread(CLoadingScreen::GetThreadID(), NULL);
	}

	//Resume all sound effects for when we return from pause
	CSoundEffect::ResumeAll();

	//Hide mouse cursor
	if (SDL_ShowCursor(SDL_DISABLE) < 0)
	{
		Warning("Unable to hide cursor", "Warning: Unable to hide cursor\n" + string(SDL_GetError()));
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

	//Pause all sound effects
	CSoundEffect::PauseAll();

	SDL_ShowCursor(SDL_ENABLE);
}

void CAppStateMain::OnExit()
{
	if (Loaded)
	{
		std::cout << "Releasing CAppStateMain\n";
		CModel::ReleaseAll();
		CInstancedModel::ReleaseAllInstanced();
		ParticleEruption.Release();	TextureParticleEruption.Release();
		ParticleSmoke.Release();	TextureParticleSmoke.Release();
		ParticleFire.Release();		TextureParticleFire.Release();
		ParticleSnow.Release();		TextureParticleSnow.Release();
		ShaderVertex.Release();
		ShaderFragment.Release();
		ShaderInstancing.Release();
		ShaderFontVertex.Release();
		ShaderFontFragment.Release();
		ProgramMain.Release();
		ProgramInstancing.Release();
		ProgramFont.Release();
		Skybox.Release();
		CHeightMap::ReleaseShaderProgram();
		for (int i = 0; i < 6; i++) TextureTerrain[i].Release();
		Map.Release();
		SoundFire.Release();
		MusicMain.Release();
		FontGunplay.Release();
		FontEthnocentric.Release();
	}
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
	Clamp(VerticalAngle, -CParams::Pi / 2.0, CParams::Pi / 2.0);
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

	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 5000.0f);

	//Get the view matrix using lookAt
	ViewMatrix = glm::lookAt(
		Position,
		Position + Direction,
		Up
		);

	OrthogonalMatrix = glm::ortho(0.0f, (float)CMain::GetInstance()->GetWindowWidth(), 0.0f, (float)CMain::GetInstance()->GetWindowHeight());

	//Set matrices for particle program
	ParticleEruption.SetMatrices(&ProjectionMatrix, &ViewMatrix, Direction);
	ParticleSmoke.SetMatrices(&ProjectionMatrix, &ViewMatrix, Direction);
	ParticleFire.SetMatrices(&ProjectionMatrix, &ViewMatrix, Direction);
	ParticleSnow.ChangePosition(glm::vec3(Position.x, Position.y + 200, Position.z));
	ParticleSnow.SetMatrices(&ProjectionMatrix, &ViewMatrix, Direction);

	float dist = Distance(FirePosition, Position);
	int vol = 1/(dist - 50) * MIX_MAX_VOLUME;
	SoundFire.SetVolume(vol);
}

void CAppStateMain::OnRender()
{
	//Clear the screen for each frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	//Use shaders
	ProgramMain.Use();

	ProgramMain.SetUniform("matrices.mProjection", &ProjectionMatrix);
	ProgramMain.SetUniform("matrices.mView", &ViewMatrix);
	ProgramMain.SetUniform("mat.diffuse", 0);
	ProgramMain.SetUniform("mat.normal", 1);
	ProgramMain.SetUniform("mat.specular", 2);
	ProgramMain.SetUniform("matrices.mModel", glm::mat4(1.0));
	ProgramMain.SetUniform("matrices.mNormal", glm::mat4(1.0));
	CFog::SetUniforms(&ProgramMain, FogEnabled);
	//Set light uniform
	Sun.SetUniform(&ProgramMain, "sunLight");

	//Render Skybox
	ProgramMain.SetUniform("matrices.mModel", glm::translate(glm::mat4(1.0), Position));
	ProgramMain.SetUniform("bSkybox", 1);
	Skybox.Render();
	ProgramMain.SetUniform("bSkybox", 0);

	//Reset model matrix
	ProgramMain.SetUniform("matrices.mModel", glm::mat4(1.0));

	//Render models
	CModel::BindVAO();
	ProgramMain.SetUniform("vEyePosition", Position);

	glm::vec3 newPos(83, 0, 180);
	newPos.y = Map.GetHeight(newPos);
	ModelMatrix = glm::translate(glm::mat4(1.0), newPos);
	ProgramMain.SetModelAndNormalMatrix("matrices.mModel", "matrices.mNormal", ModelMatrix);
	ProgramMain.SetUniform("bEnableNormalMap", true);
	models[0].Render();

	newPos = glm::vec3(64, 0, 193);
	newPos.y = Map.GetHeight(newPos);
	ModelMatrix = glm::translate(glm::mat4(1.0), newPos);
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0));
	ModelMatrix = glm::rotate(ModelMatrix, -90.0f, glm::vec3(1, 0, 0));
	ProgramMain.SetModelAndNormalMatrix("matrices.mModel", "matrices.mNormal", ModelMatrix);
	ProgramMain.SetUniform("bEnableNormalMap", models[1].NormalMap());
	models[1].Render();

	//Render instanced models
	ProgramInstancing.Use();
	ProgramInstancing.SetUniform("matrices.mProjection", ProjectionMatrix);
	ProgramInstancing.SetUniform("matrices.mView", ViewMatrix);
	ProgramInstancing.SetUniform("mat.diffuse", 0);
	ProgramInstancing.SetUniform("mat.specular", 2);
	ProgramInstancing.SetUniform("bEnableNormalMap", false);
	Sun.SetUniform(&ProgramInstancing, "sunLight");
	CFog::SetUniforms(&ProgramInstancing, FogEnabled);
	SmallTree.RenderInstanced();

	//Render ground
	CShaderProgram* Program_Terrain = CHeightMap::GetShaderProgram();
	Program_Terrain->Use();
	Program_Terrain->SetUniform("matrices.mProjection", ProjectionMatrix);
	Program_Terrain->SetUniform("matrices.mView", ViewMatrix);
	Program_Terrain->SetUniform("vEyePosition", Position);
	Program_Terrain->SetUniform("LowAlt.diffuse", 0);	Program_Terrain->SetUniform("LowAlt.specular", 1);
	Program_Terrain->SetUniform("MidAlt.diffuse", 2);	Program_Terrain->SetUniform("MidAlt.specular", 3);
	Program_Terrain->SetUniform("HighAlt.diffuse", 4);	Program_Terrain->SetUniform("HighAlt.specular", 5);
	Program_Terrain->SetModelAndNormalMatrix("matrices.mModel", "matrices.mNormal", glm::mat4(1.0));
	Program_Terrain->SetUniform("vColor", glm::vec4(1, 1, 1, 1));
	Sun.SetUniform(Program_Terrain, "sunLight");
	CFog::SetUniforms(Program_Terrain, FogEnabled);
	//Diffuse textures			//Specular textures
	TextureTerrain[0].Bind(0);	TextureTerrain[1].Bind(1);
	TextureTerrain[2].Bind(2);	TextureTerrain[3].Bind(3);
	TextureTerrain[4].Bind(4);	TextureTerrain[5].Bind(5);
	Map.Render();

	//Render particles
	float fps = CFPS::FPSControl.GetFPS();
	float FrameInterval = 1 / fps;
	TextureParticleEruption.Bind();
	ParticleEruption.Update(FrameInterval);
	ParticleEruption.Render();

	TextureParticleSmoke.Bind();
	ParticleSmoke.Update(FrameInterval);
	ParticleSmoke.Render();

	TextureParticleFire.Bind();
	ParticleFire.Update(FrameInterval);
	ParticleFire.Render();

	TextureParticleSnow.Bind();
	ParticleSnow.Update(FrameInterval);
	ParticleSnow.Render();

	//Print text
	ProgramFont.Use();
	glDisable(GL_DEPTH_TEST);
	ProgramFont.SetUniform("matrices.projMatrix", OrthogonalMatrix);
	int height = CMain::GetInstance()->GetWindowHeight();
	ProgramFont.SetUniform("vColor", glm::vec4(1.0, 1.0, 1.0, 1.0f));
	FontEthnocentric.PrintFormatted(25, 25, 18, "Shao Kun Deng | Project ASTRIA v.%s", CParams::VersionNumber);
	ProgramFont.SetUniform("vColor", glm::vec4(0.4, 0.4, 0.4, 1.0f));
	FontGunplay.PrintFormatted(20, height - 30, 18, "FPS: %d", CFPS::FPSControl.GetFPS());
	FontGunplay.Print(HelpText, 20, height - 50, 18);

	SDL_GL_SwapWindow(CMain::GetInstance()->GetWindow());
}

int CAppStateMain::OnLoad()
{
	//Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum glewInitStatus = glewInit();
	if (glewInitStatus != GLEW_OK)
	{
		Error("Loading error", (char*)(glewGetErrorString(glewInitStatus)));
		return -1;
	}

	//Disable Vsync to prevent framerate capping at refresh rate
	if (SDL_GL_SetSwapInterval(0) < 0)
	{
		Warning("Loading error", "Warning: Unable to set VSync\n" + string(SDL_GetError()));
	}

	//Clear the background as dark blue
	glClearColor(0.1f, 0.1f, 0.4f, 0.0f);

	//Enable MSAA (4xMSAA as define in the main OnInit() function in Main.cpp)
	//glEnable(GL_MULTISAMPLE);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	//glEnable(GL_LINE_SMOOTH);

	//Enable depth test
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);

	//Remove triangles which normal is not towards the camera (do not render the inside of the model)
	//glEnable(GL_CULL_FACE);

	//Load shaders and programs
	if (!ShaderVertex.Load("shaders/main_shader.vert", GL_VERTEX_SHADER))
		return -1;
	if (!ShaderFragment.Load("shaders/main_shader.frag", GL_FRAGMENT_SHADER))
		return -1;
	if (!ProgramMain.Initiate(&ShaderVertex, &ShaderFragment))
		return -1;
	if (!ShaderInstancing.Load("shaders/instancing.vert", GL_VERTEX_SHADER))
		return -1;
	if (!ProgramInstancing.Initiate(&ShaderInstancing, &ShaderFragment))
		return -1;
	if (!ShaderFontVertex.Load("shaders/ortho2D.vert", GL_VERTEX_SHADER))
		return -1;
	if (!ShaderFontFragment.Load("shaders/font2D.frag", GL_FRAGMENT_SHADER))
		return -1;
	if (!ProgramFont.Initiate(&ShaderFontVertex, &ShaderFontFragment))
		return -1;

	//Load terrain
	string TextureNames[] = { "sand.jpg", "sand_specular.jpg", "grass.jpg", "grass_specular.jpg", "snow.jpg", "snow_specular.png" };
	for (int i = 0; i < 6; i++)
	{
		TextureTerrain[i].Load_2D("gfx/" + TextureNames[i], true);
		TextureTerrain[i].SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
	}
	Map.Load(CParams::Heightmap);
	CHeightMap::LoadShaderProgram("shaders/terrain.vert", "shaders/terrain.frag");
	Map.SetSize(CParams::WorldX, CParams::WorldY, CParams::WorldZ);

	//Load models
	models[0].Load("gfx/nanosuit/nanosuit.obj");
	models[1].Load("gfx/barrel/barrel.3ds");

	SmallTree.Load("gfx/coffee_tree/TR08y.obj");

	//Generate some random positions for environment objects
	int SceneObjCount = 75;
	SceneObjPosition.reserve(SceneObjCount);
	for (int i = 0; i < SceneObjCount; i++) SceneObjPosition.push_back(glm::vec3(RandNormal()*CParams::WorldX / 2, 0, RandNormal()*CParams::WorldZ / 2));
	for (int i = 0; i < SceneObjPosition.size(); i++)	SceneObjPosition[i].y = Map.GetHeight(SceneObjPosition[i]);

	ModelMatrices.reserve(SceneObjCount);
	for (int i = 0; i < SceneObjCount; i++)
	{
		ModelMatrix = glm::translate(glm::mat4(1.0), SceneObjPosition[i]);
		ModelMatrix = glm::rotate(ModelMatrix, -90.0f, glm::vec3(1, 0, 0));
		ModelMatrices.push_back(ModelMatrix);
	}
	SmallTree.UploadMatrices(SceneObjCount, ModelMatrices.data());

	CModel::UploadVBO();

	ModelMatrices.clear(); ModelMatrices.shrink_to_fit();

	//Load particles
	TextureParticleEruption.Load_2D("gfx/particle.bmp", true);
	ParticleEruption.Init();
	FirePosition = glm::vec3(64, 0, 193);
	FirePosition.y = Map.GetHeight(FirePosition);
	ParticleEruption.Set(
		FirePosition,				//Position
		glm::vec3(-8, 15, -8),		//Minimum velocity
		glm::vec3(8, 20, 8),		//Maximum velocity
		glm::vec3(0, -10, 0),		//Acceleration
		glm::vec3(0.8, 0.3, 0.1),	//Color
		1.5f,	//Minimum lifespan in second
		2.0f,	//Maximum lifespan in second
		0.25f, 	//Size
		0.02,	//Spawn interval
		1000);	//Count i.e. number generated per frame

	TextureParticleFire.Load_2D("gfx/img/FireParticle.jpg", true);
	ParticleFire.Init();
	ParticleFire.Set(
		FirePosition,
		glm::vec3(-7, 8, -7),
		glm::vec3(7, 20, 7),
		glm::vec3(0, -8, 0),
		glm::vec3(0.3, 0.3, 0.3),
		0.5f,
		1.5f,
		2.0f,
		0.2,
		50);

	FirePosition.y += 3;
	TextureParticleSmoke.Load_2D("gfx/img/SmokeParticle.jpg", true);
	ParticleSmoke.Init();
	ParticleSmoke.Set(
		FirePosition,
		glm::vec3(-5, 8, -5),
		glm::vec3(5, 20, 5),
		glm::vec3(0, -8, 0),
		glm::vec3(0.3, 0.3, 0.5),
		1.5f,
		2.0f,
		2.0f,
		0.2,
		10);

	TextureParticleSnow.Load_2D("gfx/img/SnowParticle.png", true);
	ParticleSnow.Init();
	ParticleSnow.Set(
		glm::vec3(0, 500, 0),
		glm::vec3(-30, -20, -30),
		glm::vec3(30, -50, 30),
		glm::vec3(0, -50, 0),
		glm::vec3(0.2, 0.2, 0.2),
		50.0f,
		80.0f,
		1.0f,
		0.1f,
		30);

	//Model matrix //Identity matrix
	ModelMatrix = glm::mat4(1.0f);

	//Load the skybox
	Skybox.Load(CParams::SkyboxFolder);

	//Load sun light
	Sun = CDirectLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-0.1, -1.2, 1), 0.2f, 0.5f);

	//Enable fog
	CFog::Properties(0.005, glm::vec4(0.8, 0.8, 0.8, 1.0));

	//Load camera properties
	Position = glm::vec3(-78, 135, 76);
	Position.y = 5 + Map.GetHeight(Position);
	FoV = 45.0f;
	HorizontalAngle = 0.5f;
	VerticalAngle = -0.1f;

	MusicMain.Load("sound/PortalSelfEsteemFund.mp3");
	MusicMain.Play();

	SoundFire.Load("sound/fireplace.wav", 1);
	//This is need so we dont hear the sound effect for a split second at the very beginning
	float dist = Distance(FirePosition, Position);
	int vol = 1 / (dist - 50) * MIX_MAX_VOLUME;
	SoundFire.SetVolume(vol);
	SoundFire.Play(-1);

	FontGunplay.LoadFont("ttf/gunplay.ttf", 32);
	FontGunplay.SetShader(&ProgramFont);
	FontEthnocentric.LoadFont("ttf/ethnocentric.ttf", 32);
	FontEthnocentric.SetShader(&ProgramFont);

	return 1;
}

SDL_Texture* CAppStateMain::GetSnapshot()
{
	return snapshot;
}

void CAppStateMain::OnKeyDown(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode)
{
	string pos;
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
	case SDLK_F12:
		//Prompt current position, for debug purposes
		pos = Stringify::Float(Position.x) + " " + Stringify::Float(Position.y) + " " + Stringify::Float(Position.z) + "\n";
		Warning("Position", pos);
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

	//Sprint mode
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
		Speed = 3;
		break;

	case SDLK_f:
		FogEnabled = 1 - FogEnabled;
		break;
	case SDLK_g:
		//Toggle gravity
		GravityEnabled = !GravityEnabled;
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

	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
		Speed = 0.5;
		break;
	}
}

void CAppStateMain::OnLoseFocus()
{
	CAppStateManager::SetActiveAppState(APPSTATE_PAUSE);
}