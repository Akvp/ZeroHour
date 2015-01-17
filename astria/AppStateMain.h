#ifndef _APP_STATE_GL_H_
#define _APP_STATE_GL_H_

#include "AppState.h"
#include "Shader.h"
#include "Light.h"
#include "Skybox.h"
#include "StaticObj.h"
#include "HeightMap.h"
#include "Params.h"
#include "Model.h"
#include "ParticleSystem.h"
#include "LoadingScreen.h"

class CAppStateMain : public CAppState
{
public:
	CAppStateMain();
	static CAppStateMain* GetInstance();

	void OnActivate();
	void OnDeactivate();

	void OnExit();

	void OnEvent(SDL_Event* Event);
	void OnUpdate();
	void OnRender();

	SDL_Texture* GetSnapshot();

private:
	static CAppStateMain Instance;
	bool Loaded;
	bool OnLoad();
	SDL_GLContext glContext;

	const int OPENGL_MAJOR_VERSION = 3;
	const int OPENGL_MINOR_VERSION = 3;

	SDL_Texture* snapshot;

	//Event variables
	int Mouse_X;	//Mouse x position
	int Mouse_Y;	//Mouse y position

	bool MoveLeft, MoveRight, MoveUp, MoveDown;

	//Event functions
	void OnKeyDown(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode);
	void OnKeyUp(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode);
	void OnLoseFocus();


	//Shaders & Shader programs
	CShader MainShader_vertex;
	CShader MainShader_fragment;
	CShaderProgram MainProgram;

	//Skybox and lighting
	CSkybox Skybox;
	CDirectLight Sun;

	//Scene obj
	//CVBO VBO_Terrain;
	//GLuint VAO_Terrain;
	//CTexture Texture_Terrain;
	//CTexture Texture_Heightmap;
	CHeightMap Map;
	CTexture Texture_Terrain[5];

	CModel models[2];
	CParticleSystem Particle_Test;
	CTexture Texture_Particle;
	GLuint PolyMode;	//Used for wireframe

	//Matrices
	glm::mat4 ProjectionMatrix;
	glm::mat4 ViewMatrix;
	glm::mat4 ModelMatrix;

	//Movements and related Matrix transformations
	glm::vec3 Position;		//Position of the camera
	float HorizontalAngle;	//Horizontal viewing angle
	float VerticalAngle;	//Vertical viewing angle
	float FoV;				//Field of view

	glm::vec3 Direction;
	glm::vec3 Right;
	glm::vec3 Up;

	float Speed;			//Speed of movements
	float MouseSpeed;

	//HUD & orthogonal projections

};


#endif