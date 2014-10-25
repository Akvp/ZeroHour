#ifndef _APP_STATE_GL_H_
#define _APP_STATE_GL_H_

#include "AppState.h"
#include "Shader.h"
#include "Light.h"
#include "Skybox.h"
#include "StaticObj.h"
#include "Params.h"
#include "Model.h"

class CAppStateMain : public CAppState
{
public:
	CAppStateMain();
	static CAppStateMain* GetInstance();

	void OnActivate();
	void OnDeactivate();

	void Exit();

	void OnEvent(SDL_Event* Event);
	void OnUpdate();
	void OnRender();

private:
	static CAppStateMain Instance;
	bool Loaded;
	bool OnInit_GL();
	SDL_GLContext glContext;

	const int OPENGL_MAJOR_VERSION = 3;
	const int OPENGL_MINOR_VERSION = 3;

private:
	//Event variables
	int Mouse_X;	//Mouse x position
	int Mouse_Y;	//Mouse y position

	bool MoveLeft, MoveRight, MoveUp, MoveDown;

	//Event functions
	void OnKeyDown(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode);
	void OnKeyUp(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode);

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

	float Speed;			//Speed of movements
	float MouseSpeed;

};


#endif