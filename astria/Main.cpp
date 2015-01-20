#include "Main.h"

CMain CMain::Instance;

CMain::CMain()
{
	Running = true;
	Window_Main = NULL;
	Renderer = NULL;
	Surf_Icon = NULL;
}

CMain* CMain::GetInstance()
{
	return &Instance;
}

SDL_Renderer* CMain::GetRenderer()
{
	return Renderer;
}

SDL_Window* CMain::GetWindow()
{
	return Window_Main;
}

int CMain::GetWindowWidth()
{
	return window_width;
}

int CMain::GetWindowHeight()
{
	return window_height;
}

int CMain::OnExecute()
{
	if (!OnInit())
	{
		return -1;
	}

	SDL_Event Event;

	while (Running)
	{
		while (SDL_PollEvent(&Event))
		{
			OnEvent(&Event);
		}

		OnUpdate();

		OnRender();

	}

	OnExit();

	return 0;
}

void CMain::OnEvent(SDL_Event* Event)
{
	if (Event->type == SDL_QUIT)
	{
		Running = false;
	}
	CAppStateManager::OnEvent(Event);
}

bool CMain::OnInit()
{
	//Load parameters
	CParams::Load(CParams::File);

	srand(time(0));

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		Error("Initialization error", "SDL initialization error\n" + string(SDL_GetError()));
		return false;
	}

	//Initialize SDL_image for texture and image loading
	if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) != (IMG_INIT_PNG | IMG_INIT_JPG))
	{
		Error("Initialization error", "SDL_image initialization error\n" + string(IMG_GetError()));
		return false;
	}

	//Initialize SDL_ttf for font 
	if (TTF_Init() < 0)
	{
		Error("Initialization error", "SDL_ttf initialization error\n" + string(TTF_GetError()));
		return false;
	}

	int attrib = 0;
	//Set OpenGL 3.3 core
	attrib += SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, OPENGL_MAJOR_VERSION);
	attrib += SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, OPENGL_MINOR_VERSION);
	attrib += SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);				//Enable core profil
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);	//Backward compatibility

	attrib += SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);	//Add multisample buffer
	attrib += SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);	//4 samples per pixels for 4xMSAA

	if (attrib < 0)
	{
		Error("OpenGL attributes error", "Failed to set OpenGL attributes!\nError code: " + string(SDL_GetError()));
	}

	//Create the main window
	Window_Main = SDL_CreateWindow(CParams::WindowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, CParams::WindowWidth, CParams::WindowHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (Window_Main == NULL)
	{
		Error("Initialization error", "Window creation error\n" + string(SDL_GetError()));
		return false;
	}
	//Grab its dimension for future use
	SDL_GetWindowSize(Window_Main, &window_width, &window_height);

	//Set window icon
	if ((Surf_Icon = IMG_Load(CParams::IconImage)) == NULL)
	{
		Warning("Initialization error", "Unable to loading window icon");
	}
	SDL_SetWindowIcon(Window_Main, Surf_Icon);

	//Setup the renderer
	if ((Renderer = SDL_CreateRenderer(Window_Main, -1, SDL_RENDERER_ACCELERATED)) == NULL)
	{
		Error("Initialization error", "Unable to create renderer" + string(SDL_GetError()));
		return false;
	}
	SDL_SetRenderDrawColor(Renderer, 0x00, 0x00, 0x10, 0xFF);

	//Create OpenGL context
	glContext = SDL_GL_CreateContext(Window_Main);
	if (glContext == NULL)
	{
		Error("Initialization error", "OpenGL context creation failed\n" + string(SDL_GetError()));
		return false;
	}

	CAppStateManager::SetActiveAppState(APPSTATE_INTRO);

	return true;
}

void CMain::OnUpdate()
{
	CAppStateManager::OnUpdate();

	//FPSControl loop to update FPS
	CFPS::FPSControl.Loop();

	//Reset window title to add current FPS
	std::string Text_FPS = std::string(CParams::WindowName) + "    FPS: " + std::to_string(CFPS::FPSControl.GetFPS());
	SDL_SetWindowTitle(Window_Main, Text_FPS.c_str());
}

void CMain::OnRender()
{
	CAppStateManager::OnRender();
}

void CMain::OnExit()
{
	CAppStateManager::SetActiveAppState(APPSTATE_NONE);

	CAppStateMain::GetInstance()->OnExit();
	CAppStatePause::GetInstance()->OnExit();

	SDL_DestroyWindow(Window_Main);
	Window_Main = NULL;
	SDL_FreeSurface(Surf_Icon);
	Surf_Icon = NULL;
	SDL_DestroyRenderer(Renderer);
	Renderer = NULL;
	SDL_GL_DeleteContext(glContext);

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* argv[])
{
	return CMain::GetInstance()->OnExecute();
}