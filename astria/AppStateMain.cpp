#include "AppStateManager.h"
#include "AppStateMain.h"
#include "Main.h"
#include "Stringify.h"

CAppStateMain CAppStateMain::Instance;

CAppStateMain::CAppStateMain()
{
	Loaded = 0;
	snapshot = NULL;
	ProgramTerrain = NULL;
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
		FogEnabled = true;
		NormalMapEnabled = true;
		ShadowMapEnabled = true;
		Wireframe = false;
		CLoadingScreen::OnActivate(&Loaded);
		Loaded = OnLoad();
		if (!Loaded)	CMain::GetInstance()->Running = false;
		SDL_WaitThread(CLoadingScreen::GetThreadID(), NULL);
	}

	//Resume all sound effects for when we return from pause
	if(!SfxPaused) SoundFire.Resume();

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
	SoundFire.Pause();

	SDL_ShowCursor(SDL_ENABLE);
}

void CAppStateMain::OnExit()
{
	if (Loaded)
	{
		//Release models
		CModel::ReleaseAll();
		CInstancedModel::ReleaseAllInstanced();

		//Release particles
		ParticleEruption.Release();
		ParticleSmoke.Release();
		ParticleFire.Release();
		ParticleSnow.Release();

		//Release shaders and programs
		ShaderVertex.Release();
		ShaderFragment.Release();
		ShaderInstancing.Release();
		ShaderFontVertex.Release();
		ShaderFontFragment.Release();
		ProgramMain.Release();
		ProgramInstancing.Release();
		ProgramFont.Release();

		//Release skybox
		Skybox.Release();

		//Release shadow
		FBOShadowMap.Release();
		ShadowMapFragment.Release();
		ShaderFontVertex.Release();
		ProgramShadowMap.Release();

		//Release heightmap
		CHeightMap::ReleaseShaderProgram();
		for (int i = 0; i < 6; i++) TextureTerrain[i].Release();
		Map.Release();
		
		//Release music
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
	Clamp(VerticalAngle, (float)(-CParams::Pi / 2.0), (float)(CParams::Pi / 2.0));
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

	float dist = glm::distance(FirePosition, Position);
	int vol = int(1/(dist - 50.0f) * MIX_MAX_VOLUME);
	SoundFire.SetVolume(vol);
}

void CAppStateMain::OnRender()
{
	//Generate shadow map
	if (ShadowMapEnabled)
	{
		GenerateShadowMap();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, CMain::GetInstance()->GetWindowWidth(), CMain::GetInstance()->GetWindowHeight());

	//Clear the screen for each frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

	//Bind shadow maps for ProgramMain
	ProgramMain.SetUniform("matrices.mDepthBiasMVP", DepthBiasMVP);
	ProgramMain.SetUniform("bEnableShadowMap", ShadowMapEnabled);
	ProgramMain.SetUniform("sShadowMap", 6);
	FBOShadowMap.BindTexture(6);

	//Render models
	CModel::BindVAO();
	ProgramMain.SetUniform("vEyePosition", Position);

	glm::vec3 newPos(83, 0, 180);
	newPos.y = Map.GetHeight(newPos);
	ModelMatrix = glm::translate(glm::mat4(1.0), newPos);
	ProgramMain.SetModelAndNormalMatrix("matrices.mModel", "matrices.mNormal", ModelMatrix);
	ProgramMain.SetUniform("bEnableNormalMap", NormalMapEnabled);
	models[0].Render();

	newPos = glm::vec3(64, 0, 193);
	newPos.y = Map.GetHeight(newPos);
	ModelMatrix = glm::translate(glm::mat4(1.0), newPos);
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0));
	ModelMatrix = glm::rotate(ModelMatrix, -90.0f, glm::vec3(1, 0, 0));
	ProgramMain.SetModelAndNormalMatrix("matrices.mModel", "matrices.mNormal", ModelMatrix);
	ProgramMain.SetUniform("bEnableNormalMap", NormalMapEnabled);
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
	ProgramTerrain = CHeightMap::GetShaderProgram();
	ProgramTerrain->Use();
	ProgramTerrain->SetUniform("matrices.mProjection", ProjectionMatrix);
	ProgramTerrain->SetUniform("matrices.mView", ViewMatrix);
	ProgramTerrain->SetUniform("vEyePosition", Position);
	ProgramTerrain->SetUniform("LowAlt.diffuse", 0);	ProgramTerrain->SetUniform("LowAlt.specular", 1);
	ProgramTerrain->SetUniform("MidAlt.diffuse", 2);	ProgramTerrain->SetUniform("MidAlt.specular", 3);
	ProgramTerrain->SetUniform("HighAlt.diffuse", 4);	ProgramTerrain->SetUniform("HighAlt.specular", 5);
	ProgramTerrain->SetModelAndNormalMatrix("matrices.mModel", "matrices.mNormal", glm::mat4(1.0));
	ProgramTerrain->SetUniform("vColor", glm::vec4(1, 1, 1, 1));
	Sun.SetUniform(ProgramTerrain, "sunLight");
	CFog::SetUniforms(ProgramTerrain, FogEnabled);
	//Diffuse textures			//Specular textures
	TextureTerrain[0].Bind(0);	TextureTerrain[1].Bind(1);
	TextureTerrain[2].Bind(2);	TextureTerrain[3].Bind(3);
	TextureTerrain[4].Bind(4);	TextureTerrain[5].Bind(5);
	//Bind shadow map texture
	ProgramTerrain->SetUniform("matrices.mDepthBiasMVP", DepthBiasMVP);
	ProgramTerrain->SetUniform("bEnableShadowMap", ShadowMapEnabled);
	ProgramTerrain->SetUniform("sShadowMap", 6);
	FBOShadowMap.BindTexture(6);
	Map.Render();

	//Render particles
	float fps = (float)(CFPS::FPSControl.GetFPS());
	float FrameInterval = 1 / fps;
	ParticleEruption.Update(FrameInterval);
	ParticleEruption.Render();
	ParticleSmoke.Update(FrameInterval);
	ParticleSmoke.Render();
	ParticleFire.Update(FrameInterval);
	ParticleFire.Render();
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
	FontGunplay.PrintFormatted(20, height - 50, 18, "Normal map: %s", NormalMapEnabled ? "Enabled" : "Disabled");
	FontGunplay.Print(HelpText, 20, height - 70, 18);
	glEnable(GL_DEPTH_TEST);

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

	std::string ParticleShaders[] =
	{
		"shaders/particles_update.vert",
		"shaders/particles_update.geom",
		"shaders/particles_render.vert",
		"shaders/particles_render.geom",
		"shaders/particles_render.frag"
	};

	//Load particles
	ParticleEruption.Init(ParticleShaders);
	ParticleEruption.SetTexture("gfx/particle.bmp");
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
		0.02f,	//Spawn interval
		1000);	//Count i.e. number generated per frame

	ParticleFire.Init(ParticleShaders);
	ParticleFire.SetTexture("gfx/img/FireParticle.jpg");
	ParticleFire.Set(
		FirePosition,
		glm::vec3(-7, 8, -7),
		glm::vec3(7, 20, 7),
		glm::vec3(0, -8, 0),
		glm::vec3(0.3, 0.3, 0.3),
		0.5f,
		1.5f,
		2.0f,
		0.2f,
		50);

	FirePosition.y += 3;
	ParticleSmoke.Init(ParticleShaders);
	ParticleSmoke.SetTexture("gfx/img/SmokeParticle.jpg");
	ParticleSmoke.Set(
		FirePosition,
		glm::vec3(-5, 8, -5),
		glm::vec3(5, 20, 5),
		glm::vec3(0, -8, 0),
		glm::vec3(0.3, 0.3, 0.5),
		1.5f,
		2.0f,
		2.0f,
		0.2f,
		10);

	ParticleSnow.Init(ParticleShaders);
	ParticleSnow.SetTexture("gfx/img/SnowParticle.png");
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

	//Initiate model matrix
	ModelMatrix = glm::mat4(1.0f);

	//Load the skybox
	Skybox.Load(CParams::SkyboxFolder);
	//Load sun light
	Sun = CDirectLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-0.1f, -1.2f, 1.0f), 0.2f, 0.5f);
	//Enable fog
	CFog::Properties(0.003f, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));

	//Initiate shadow maps
	if (!InitiateShadowMap())
		return -1;
	
	//Load camera properties
	Position = glm::vec3(-78, 135, 76);
	Position.y = 5 + Map.GetHeight(Position);
	FoV = 45.0f;
	HorizontalAngle = 0.5f;
	VerticalAngle = -0.1f;

	MusicPaused = SfxPaused = false;
	MusicMain.Load("sound/PortalSelfEsteemFund.mp3");
	MusicMain.Play();

	SoundFire.Load("sound/fireplace.wav", 1);
	//This is need so we dont hear the sound effect for a split second at the very beginning
	float dist = glm::distance(FirePosition, Position);
	int vol = (int)(1 / (dist - 50) * MIX_MAX_VOLUME);
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

bool CAppStateMain::InitiateShadowMap()
{
	ShadowMapSize = 1024;

	if (!FBOShadowMap.CreateWithTexture(ShadowMapSize, ShadowMapSize))
		return false;
	if (!FBOShadowMap.AddDepthBuffer())
		return false;
	FBOShadowMap.SetTextureFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_NEAREST);

	if (!ShadowMapVertex.Load("shaders/shadowMapper.vert", GL_VERTEX_SHADER))
		return false;
	if (!ShadowMapFragment.Load("shaders/shadowMapper.frag", GL_FRAGMENT_SHADER))
		return false;
	if (!ProgramShadowMap.Initiate(&ShadowMapVertex, &ShadowMapFragment))
		return false;

	return true;
}

void CAppStateMain::GenerateShadowMap()
{	
	FBOShadowMap.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ProgramShadowMap.Use();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	ProgramShadowMap.Use();

	const float RangeX = 500, RangeY = 500, MinZ = 0.05f, MaxZ = 400;
	glm::mat4 ProjShadow = glm::ortho<float>(-RangeX, RangeX, RangeY, -RangeY, MinZ, MaxZ);
	glm::vec3 LightPosition = glm::vec3(0.0f, 0.0f, 0.0f)-Sun.Direction*500.0f;	//Since we have directional, we just make the light very far so it can see everything
	glm::mat4 ViewShadow = glm::lookAt(LightPosition, glm::vec3(0, 0, 0) - LightPosition, glm::vec3(0, 1, 0));
	glm::mat4 depthMVP;

	glm::mat4 BiasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
		);

	//Calculate depth bias matrix
	DepthBiasMVP = BiasMatrix * ProjShadow * ViewShadow;

	//Render models
	CModel::BindVAO();

	glm::vec3 newPos(83, 0, 180);
	newPos.y = Map.GetHeight(newPos);
	ModelMatrix = glm::translate(glm::mat4(1.0), newPos);
	ProgramShadowMap.SetModelAndNormalMatrix("matrices.mModel", "matrices.mNormal", ModelMatrix);
	depthMVP = ProjShadow * ViewShadow * ModelMatrix;
	ProgramShadowMap.SetUniform("depthMVP", depthMVP);
	models[0].Render();

	//newPos = glm::vec3(64, 0, 193);
	//newPos.y = Map.GetHeight(newPos);
	//ModelMatrix = glm::translate(glm::mat4(1.0), newPos);
	//ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.0));
	//ModelMatrix = glm::rotate(ModelMatrix, -90.0f, glm::vec3(1, 0, 0));
	//ProgramShadowMap.SetModelAndNormalMatrix("matrices.mModel", "matrices.mNormal", ModelMatrix);
	//depthMVP = ProjShadow * ViewShadow * ModelMatrix;
	//ProgramShadowMap.SetUniform("depthMVP", depthMVP);
	//models[1].Render();

	//Render instanced models
	//CModel::BindVAO();
	//ProgramInstancing.Use();
	//ProgramInstancing.SetUniform("matrices.mProjection", ProjectionMatrix);
	//ProgramInstancing.SetUniform("matrices.mView", ViewMatrix);
	//ProgramInstancing.SetUniform("mat.diffuse", 0);
	//ProgramInstancing.SetUniform("mat.specular", 2);
	//Sun.SetUniform(&ProgramInstancing, "sunLight");
	//CFog::SetUniforms(&ProgramInstancing, FogEnabled);
	//SmallTree.RenderInstanced();

	//Render ground
	//ModelMatrix = glm::mat4(1.0f);
	//depthMVP = ProjShadow * ViewShadow * ModelMatrix;
	//ProgramShadowMap.SetUniform("depthMVP", depthMVP);
	//Map.Render();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

	//Toggles
	case SDLK_f:
		//Toggle fog
		FogEnabled = !FogEnabled;
		break;
	case SDLK_g:
		//Toggle gravity
		GravityEnabled = !GravityEnabled;
		break;
	case SDLK_m:
		//Toggle music
		if (MusicPaused) { MusicMain.Resume(); MusicPaused = false;	}
		else { MusicMain.Pause(); MusicPaused = true; }
		if (SfxPaused) { SoundFire.Resume(); SfxPaused = false; }
		else { SoundFire.Pause(); SfxPaused = true; }
		break;
	case SDLK_n:
		//Toggle normal map
		NormalMapEnabled = !NormalMapEnabled;
		break;
	case SDLK_t:
		//Toggle shadow map
		ShadowMapEnabled = !ShadowMapEnabled;
		break;
	case SDLK_q:
		//toggle wireframe mode
		if (Wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case SDLK_p:
		FBOShadowMap.GetTexture()->Save("shadowmap.tga");
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