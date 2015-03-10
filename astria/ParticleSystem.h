#ifndef _PARTICLE_SYSTEM_H_
#define _PARTICLE_SYSTEM_H_

#include "Common.h"
#include "Shader.h"
#include "Texture.h"

enum
{
	PARTICLE_TYPE_GENERATOR,
	PARTICLE_TYPE_NORMAL
};

//#define PARTICLE_TYPE_GENERATOR 0
//#define PARTICLE_TYPE_NORMAL 1

#define PARTICLE_NUM_ATTRIBUTE 6

//
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

	bool Init(std::string* shaders);
	bool Init(std::string update_vert, std::string update_geom, std::string render_vert, std::string render_geom, std::string render_frag);

	void Render();
	void Update(float time);

	void SetMaxParticles(int count);

	void Set(glm::vec3 position, glm::vec3 velocitymin, glm::vec3 velocitymax, glm::vec3 acceleration, glm::vec3 color, float lifemin, float lifemax, float size, float interval, int count);
	void SetTexture(std::string file);
	void SetTexture(CTexture* texture);

	void ChangePosition(glm::vec3 position);

	void Clear();
	bool Release();

	int GetNumParticles();
	void SetMatrices(glm::mat4* projection, glm::mat4* view, glm::vec3 direction);

private:
	static int MAX_PARTICLES;

	bool Initialized;
	GLuint TransformFeedbackBuffer;

	GLuint ParticleBuffer[2];
	GLuint VAO[2];

	GLuint Query;
	CTexture* Texture;

	int CurrentReadBuffer;
	int NumParticles;

	glm::mat4 Projection, View;
	glm::vec3 Quad1, Quad2;

	float ElapsedTime;
	float NextGenerationTime;

	glm::vec3 Position, VelocityMin, VelocityRange, Acceleration, Color;

	float LifeMin, LifeRange;
	float Size;

	int Count;

	CShader Render_Vertex, Render_Geometry, Render_Fragment;
	CShader Update_Vertex, Update_Geometry, Update_Fragment;
	CShaderProgram Program_Render, Program_Update;
};

#endif