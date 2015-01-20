#ifndef _WATER_H_
#define _WATER_H_

#include "Common.h"
#include "Texture.h"
#include "Shader.h"
#include "VBO.h"

class CWaterPlane
{
public: 
	CWaterPlane();

	bool Load(string normal, string derivative, glm::vec3 position, float sizex, float sizez);
	void Render();
	void Release();

	static bool LoadProgram(string vertex, string fragment);
	static void ReleaseProgram();

	static CShaderProgram* GetProgram();

private:
	glm::vec3 Position;
	float SizeX, SizeZ;

	GLuint VAO;
	CVBO VBO;

	//Textures
	CTexture TextureNormal;
	CTexture TextureDerivative;

	//Shaders & Programs
	static CShader ShaderVertex;
	static CShader ShaderFragment;
	static CShaderProgram ProgramMain;
};

#endif