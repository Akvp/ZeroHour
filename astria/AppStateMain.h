#ifndef _APP_STATE_GL_H_
#define _APP_STATE_GL_H_

#include "AppState.h"
#include "Common.h"
#include "Shader.h"
#include "Light.h"
#include "Skybox.h"
#include "HeightMap.h"
#include "Model.h"
#include "Water.h"
#include "ParticleSystem.h"
#include "LoadingScreen.h"
#include "Sound.h"
#include "Font.h"
#include "Fog.h"
#include "ShadowMap.h"

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
	//Private methods
	int OnLoad();
	bool InitiateShadowMap();
	void GenerateShadowMap();
	//Event functions
	void OnKeyDown(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode);
	void OnKeyUp(SDL_Keycode sym, Uint16 mod, SDL_Scancode scancode);
	void OnLoseFocus();

private:
	static CAppStateMain Instance;
	int Loaded;
	SDL_GLContext glContext;
	bool Wireframe;

	const int OPENGL_MAJOR_VERSION = 3;
	const int OPENGL_MINOR_VERSION = 3;

	SDL_Texture* snapshot;

	//Event variables
	int Mouse_X;	//Mouse x position
	int Mouse_Y;	//Mouse y position
	bool MoveLeft, MoveRight, MoveUp, MoveDown;

	//Shaders & Shader programs
	CShader ShaderVertex;
	CShader ShaderFragment;
	CShaderProgram ProgramMain;
	CShader ShaderInstancing;
	CShaderProgram ProgramInstancing;
	CShader ShaderFontVertex;
	CShader ShaderFontFragment;
	CShaderProgram ProgramFont;
	CShaderProgram* ProgramTerrain;

	//Environment
	CSkybox Skybox;
	CDirectLight Sun;
	
	//Shadow mapping
	CFBO FBOShadowMap;
	int ShadowMapSize;
	CShader ShadowMapVertex;
	CShader ShadowMapFragment;
	CShaderProgram ProgramShadowMap;
	glm::mat4 DepthBiasMVP;

	//Scene obj
	CHeightMap Map;
	CTexture TextureTerrain[6];

	//Models
	CModel models[2];
	CInstancedModel SmallTree;
	std::vector<glm::vec3> SceneObjPosition;
	std::vector<glm::mat4> ModelMatrices;

	//Particles
	glm::vec3 FirePosition;
	CParticleSystem ParticleEruption;
	CParticleSystem ParticleSmoke;
	CParticleSystem ParticleFire;
	CParticleSystem ParticleSnow;

	//Matrices
	glm::mat4 ProjectionMatrix;
	glm::mat4 ViewMatrix;
	glm::mat4 ModelMatrix;
	glm::mat4 OrthogonalMatrix;

	//Movements and related Matrix transformations
	glm::vec3 Position;		//Position of the camera
	float HorizontalAngle;	//Horizontal viewing angle
	float VerticalAngle;	//Vertical viewing angle
	float FoV;				//Field of view

	//Camera coordinates
	glm::vec3 Direction;
	glm::vec3 Right;
	glm::vec3 Up;

	//Toggles
	bool GravityEnabled;
	bool FogEnabled;
	bool NormalMapEnabled;
	bool ShadowMapEnabled;
	
	//Control parameters
	float Speed;			//Speed of movements
	float MouseSpeed;

	//Music and sound effects
	CMusic MusicMain;
	CSoundEffect SoundFire;
	bool MusicPaused, SfxPaused;

	//Font
	Text_GL FontGunplay;
	Text_GL FontEthnocentric;
};


#endif