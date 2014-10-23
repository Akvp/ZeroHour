//The main header, contains most of the program flow functions


#ifndef _H_MAIN_
#define _H_MAIN_

#include "Common.h"
#include "Log.h"

#include "Model.h"
#include "Event.h"
#include "FPS.h"
#include "Shader.h"
#include "Skybox.h"
#include "Light.h"
#include "StaticObj.h"
#include "Params.h"

#include "AppStateManager.h"

#include "Agent.h"

#define _DEBUG 1

class CMain : public CEvent
{
public:
	CMain();
	bool Running;

	static CMain* GetInstance();

	SDL_Renderer* GetRenderer();
	SDL_Window* GetWindow();

	//Program entry point
	int OnExecute();

	//Initiate the program
	bool OnInit();
	//Initiate graphics and OpenGL related contents
	bool OnInit_GL();

	//Handle Events
	void OnEvent(SDL_Event* Event);

	//Update on each frame
	void OnUpdate();

	//Handle rendering
	void OnRender();

	//Cleanup the program upon exiting
	void OnExit();

private:
	static CMain Instance;

	SDL_Window* Window_Main;
	SDL_GLContext glContext;
	SDL_Renderer* Renderer;

	const int OPENGL_MAJOR_VERSION = 3;
	const int OPENGL_MINOR_VERSION = 3;

private:
	//Event variables
	int Mouse_X;	//Mouse x position
	int Mouse_Y;	//Mouse y position
};

#endif