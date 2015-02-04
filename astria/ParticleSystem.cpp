#include "ParticleSystem.h"

CParticleSystem::CParticleSystem()
{
	Initialized = false;
	CurrentReadBuffer = 0;
}

bool CParticleSystem::Init()
{
	if (Initialized)
	{
		return false;
	}

	const char* Varying[PARTICLE_NUM_ATTRIBUTE] =
	{
		"vPositionOut",
		"vVelocityOut",
		"vColorOut",
		"fLifeTimeOut",
		"fSizeOut",
		"iTypeOut",
	};

	//Load update program
	Update_Vertex.Load("shaders/particles_update.vert", GL_VERTEX_SHADER);
	Update_Geometry.Load("shaders/particles_update.geom", GL_GEOMETRY_SHADER);
	//Update_Fragment.Load("shaders/particles_update.frag", GL_FRAGMENT_SHADER);

	Program_Update.Create();
	Program_Update.AddShader(&Update_Vertex);
	Program_Update.AddShader(&Update_Geometry);
	//Program_Update.AddShader(&Update_Fragment);
	for (int i = 0; i < PARTICLE_NUM_ATTRIBUTE; i++)
		glTransformFeedbackVaryings(Program_Update.GetID(), 6, Varying, GL_INTERLEAVED_ATTRIBS);
	Program_Update.Link();

	//Load render program
	Render_Vertex.Load("shaders/particles_render.vert", GL_VERTEX_SHADER);
	Render_Geometry.Load("shaders/particles_render.geom", GL_GEOMETRY_SHADER);
	Render_Fragment.Load("shaders/particles_render.frag", GL_FRAGMENT_SHADER);

	Program_Render.Initiate(3, &Render_Vertex, &Render_Geometry, &Render_Fragment);

	glGenTransformFeedbacks(1, &TransformFeedbackBuffer);
	glGenQueries(1, &Query);

	glGenBuffers(2, ParticleBuffer);
	glGenVertexArrays(2, VAO);

	SParticle Initialization;
	Initialization.Type = 0;

	for (int i = 0; i < 2; i++)
	{
		glBindVertexArray(VAO[i]);
		glBindBuffer(GL_ARRAY_BUFFER, ParticleBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(SParticle)*MAX_PARTICLES, NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(SParticle), &Initialization);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SParticle), (const GLvoid*)0); // Position
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SParticle), (const GLvoid*)12); // Velocity
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(SParticle), (const GLvoid*)24); // Color
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(SParticle), (const GLvoid*)36); // Lifetime
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(SParticle), (const GLvoid*)40); // Size
		glVertexAttribPointer(5, 1, GL_INT, GL_FALSE, sizeof(SParticle), (const GLvoid*)44); // Type
	}

	CurrentReadBuffer = 0;
	NumParticles = 1;
	Initialized = true;
	return true;
}

void CParticleSystem::Update(float time)
{
	if (!Initialized)	return;
	
	Program_Update.Use();

	Program_Update.SetUniform("fTimePassed", time);
	Program_Update.SetUniform("vGenPosition", Position);
	Program_Update.SetUniform("vGenVelocityMin", VelocityMin);
	Program_Update.SetUniform("vGenVelocityRange", VelocityRange);
	Program_Update.SetUniform("vGenColor", Color);
	Program_Update.SetUniform("vAcceleration", Acceleration);
	Program_Update.SetUniform("fGenLifeMin", LifeMin);
	Program_Update.SetUniform("fGenLifeRange", LifeRange);
	Program_Update.SetUniform("fGenSize", Size);
	Program_Update.SetUniform("iNumToGenerate", 0);

	ElapsedTime += time;

	if (ElapsedTime > NextGenerationTime)
	{
		Program_Update.SetUniform("iNumToGenerate", Count);
		ElapsedTime -= NextGenerationTime;
		glm::vec3 seed = glm::vec3(RandFloatInterval(-10.0, 10.0), RandFloatInterval(-10.0, 10.0), RandFloatInterval(-10.0, 10.0));
		Program_Update.SetUniform("vRandomSeed", &seed);
	}

	glEnable(GL_RASTERIZER_DISCARD);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, TransformFeedbackBuffer);

	glBindVertexArray(VAO[CurrentReadBuffer]);
	glEnableVertexAttribArray(1);	//Enable velocity

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, ParticleBuffer[1 - CurrentReadBuffer]);

	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, Query);
	glBeginTransformFeedback(GL_POINTS);

	glDrawArrays(GL_POINTS, 0, NumParticles);

	glEndTransformFeedback();

	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
	glGetQueryObjectiv(Query, GL_QUERY_RESULT, &NumParticles);

	CurrentReadBuffer = 1 - CurrentReadBuffer;
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

void CParticleSystem::Render()
{
	if (!Initialized)	return;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(0);

	glDisable(GL_RASTERIZER_DISCARD);
	Program_Render.Use();
	Program_Render.SetUniform("matrices.mProj", &Projection);
	Program_Render.SetUniform("matrices.mView", &View);
	Program_Render.SetUniform("vQuad1", &Quad1);
	Program_Render.SetUniform("vQuad2", &Quad2);
	Program_Render.SetUniform("gSampler", 0);

	glBindVertexArray(VAO[CurrentReadBuffer]);
	glDisableVertexAttribArray(1);	//Disable velocity

	glDrawArrays(GL_POINTS, 0, NumParticles);

	glDepthMask(1);
	glDisable(GL_BLEND);
}

void CParticleSystem::ChangePosition(glm::vec3 position)
{
	Position = position;
}

void CParticleSystem::Clear()
{

}

bool CParticleSystem::Release()
{
	if (!Initialized) return false;

	Program_Update.Release();
	Program_Render.Release();

	Update_Vertex.Release();
	Update_Geometry.Release();
	Update_Fragment.Release();
	Render_Vertex.Release();
	Render_Geometry.Release();
	Render_Fragment.Release();

	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, ParticleBuffer);
	glDeleteTransformFeedbacks(1, &TransformFeedbackBuffer);

	Initialized = false;
	return true;
}

void CParticleSystem::SetMatrices(glm::mat4* projection, glm::mat4* view, glm::vec3 direction)
{
	Projection = *projection;
	View = *view;

	Quad1 = glm::cross(direction, glm::vec3(0, 1, 0));
	if (fabsf(direction.y) < 0.0001f) Quad1 = glm::cross(direction, glm::vec3(1, 0, 0));
	Quad1 = glm::normalize(Quad1);
	Quad2 = glm::cross(direction, Quad1);
	Quad2 = glm::normalize(Quad2);
} 

void CParticleSystem::Set(glm::vec3 position, glm::vec3 velocitymin, glm::vec3 velocitymax, glm::vec3 acceleration, glm::vec3 color, float lifemin, float lifemax, float size, float interval, int count)
{
	Position = position;
	VelocityMin = velocitymin;
	VelocityRange = velocitymax - velocitymin;

	Acceleration = acceleration;
	Color = color;
	Size = size;

	LifeMin = lifemin;
	LifeRange = lifemax - lifemin;

	NextGenerationTime = interval;
	ElapsedTime = 0.8f;

	Count = count;
}

int CParticleSystem::GetNumParticles()
{
	return NumParticles;
}