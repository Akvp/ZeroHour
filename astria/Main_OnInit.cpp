#include "Main.h"

bool CMain::OnInit()
{
	//Load parameters
	CParams::Load(CParams::File);

	//Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL initiation error", SDL_GetError(), NULL);
		return false;
	}

#ifdef _DEBUG
	//Enable console output
	freopen("CON", "w", stdout); // redirects stdout
	freopen("CON", "w", stderr); // redirects stderr
#endif

	//Initialize SDL_image for texture and image loading
	if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL_image initiation", IMG_GetError(), NULL);
		return false;
	}
	
	//Set OpenGL 3.3 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, OPENGL_MAJOR_VERSION);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, OPENGL_MINOR_VERSION);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);				//Enable core profil
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);	//Backward compatibility

	//User interaction before the creation of the main window
	//- Fullscreen
	int FullscreenOption = MessageBox(NULL, "Enable fullscreen?\nP.S. Border less window full screen", "Fullscreen", MB_ICONQUESTION | MB_YESNO);

	//Create Window
	switch (FullscreenOption)
	{
	case IDNO:
		Window_Main = SDL_CreateWindow(CParams::WindowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, CParams::WindowWidth, CParams::WindowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		break;
	case IDYES:
		Window_Main = SDL_CreateWindow(CParams::WindowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, CParams::WindowWidth, CParams::WindowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
		break;
	}
	if(Window_Main == NULL)
	{
		MessageBox(NULL, SDL_GetError(), "Window creation error", MB_ICONERROR);
		return false;
	}

	//Create OpenGL context
	glContext = SDL_GL_CreateContext(Window_Main);
	if(glContext == NULL)
	{
		MessageBox(NULL, SDL_GetError(), "OpenGL context creation failed!", MB_ICONERROR);
		return false;
	}

	//Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum glewInitStatus = glewInit();
	if(glewInitStatus != GLEW_OK)
	{
		MessageBox(NULL, (char*)(glewGetErrorString(glewInitStatus)), "GLEW initialization failed!", MB_ICONERROR);
		return false;
	}

	//Enable Vsync
	if (SDL_GL_SetSwapInterval(1) < 0)
	{
		MessageBox(NULL, SDL_GetError(), "Warning: Unable to set VSync!", MB_ICONWARNING);
	}
	

	//Initialize OpenGL components
	if(!OnInit_GL())
	{
		return false;
	}

	//Hide mouse cursor
	if (SDL_ShowCursor(SDL_DISABLE) < 0)
	{
		MessageBox(NULL, SDL_GetError(), "Warning: Unable to hide cursor", MB_ICONWARNING);
	}

	//Center mouse cursor
	SDL_WarpMouseInWindow(Window_Main, CParams::WindowWidth / 2, CParams::WindowHeight / 2);

	return true;
}

bool CMain::OnInit_GL()
{
	//Clear the background as dark blue
	glClearColor(0.1f, 0.1f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
	//glDepthFunc(GL_LESS);

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
				CParams::SkyboxTop,
				CParams::SkyboxBot,
				CParams::SkyboxLeft,
				CParams::SkyboxRight,
				CParams::SkyboxFront,
				CParams::SkyboxBack);

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