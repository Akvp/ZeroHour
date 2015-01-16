#ifndef _PARTICLE_SYSTEM_H_
#define _PARTICLE_SYSTEM_H_

#include "Common.h"
#include "Shader.h"
#include "utils.h"

enum
{
	PARTICLE_TYPE_GENERATOR,
	PARTICLE_TYPE_NORMAL
};

//#define PARTICLE_TYPE_GENERATOR 0
//#define PARTICLE_TYPE_NORMAL 1

#define PARTICLE_NUM_ATTRIBUTE 6

struct SParticle
{
	glm::vec3 Position;
	glm::vec3 Velocity;
	glm::vec3 Color;
	float LifeSpan;
	float Size;
	int Type;
};

class CParticleSystem
{
public:
	CParticleSystem();

	bool Init();

	void Render();
	void Update(float time);

	void Set(glm::vec3 position, glm::vec3 velocitymin, glm::vec3 velocitymax, glm::vec3 gravity, glm::vec3 color, float lifemin, float lifemax, float size, float interval, int count);

	void Clear();
	bool Release();

	int GetNumParticles();
	void SetMatrices(glm::mat4* projection, glm::mat4* view, glm::vec3 direction);

private:
	static const int MAX_PARTICLES = 100000;

	bool Initialized;
	GLuint TransformFeedbackBuffer;

	GLuint ParticleBuffer[2];
	GLuint VAO[2];

	GLuint Query;
	GLuint Texture;

	int CurrentReadBuffer;
	int NumParticles;

	glm::mat4 Projection, View;
	glm::vec3 Quad1, Quad2;

	float ElapsedTime;
	float NextGenerationTime;

	glm::vec3 Position, VelocityMin, VelocityRange, Gravity, Color;

	float LifeMin, LifeRange;
	float Size;

	int Count;

	CShader Render_Vertex, Render_Geometry, Render_Fragment;
	CShader Update_Vertex, Update_Geometry, Update_Fragment;
	CShaderProgram Program_Render, Program_Update;
};

#endif