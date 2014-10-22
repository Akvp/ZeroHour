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

#include "Agent.h"

#define _DEBUG 1

class CMain : public CEvent
{
public:
	CMain();

	static CMain* GetInstance();

	SDL_Renderer* GetRenderer();

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
	bool Running;
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

	//Event functions
	void OnKeyDown(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode);

private:	
	//Shaders & Shader programs
	CShader mainShader_vertex;
	CShader mainShader_fragment;
	//CShader ortho2D_vertex;
	//CShader ortho2D_fragment;
	CShader lightShader_fragment;
	CShaderProgram mainProgram;

	//Skybox and lighting
	CSkybox skybox;
	CDirectLight sun;

	//Scene obj
	CVBO	scene_VBO;
	GLuint	scene_VAO;
	CTexture scene_texture;	//Texture for static scene object
	CTexture test_texture;

	CModel models[2];	//Model array
	GLuint PolyMode;	//Used for wireframe

	//Matrices
	glm::mat4 Projection;
	glm::mat4 View;
	glm::mat4 Model;
	glm::mat4 MVP;
	glm::mat4 ModelView;

private:	//Movements and related Matrix transformations
	glm::vec3 Position;		//Position of the camera
	float HorizontalAngle;	//Horizontal viewing angle
	float VerticalAngle;	//Vertical viewing angle
	float FoV;				//Field of view

	glm::vec3 Direction;
	glm::vec3 Right;

	Agent* agent;

	float Speed;			//Speed of movements
	float MouseSpeed;
};

#endif