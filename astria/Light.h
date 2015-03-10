#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <string>

#include "Common.h"
#include "Shader.h"

class CDirectLight
{
public:
	glm::vec3 Color;	//color of light
	glm::vec3 Direction;

	float Ambient;
	float Brightness;

	void SetUniform(CShaderProgram* program, std::string lightVarName);

	CDirectLight();
	CDirectLight(glm::vec3 color, glm::vec3 direction, float ambient, float brightness);
};

class CPointLight
{
public:
	glm::vec3 Color;
	glm::vec3 Position;
	float Ambient;

	float ConstantAttenuation;
	float LinearAttenuation;
	float ExponentialAttenuation;

	void SetUniform(CShaderProgram* program, std::string lightVarName);

	CPointLight();
	CPointLight(glm::vec3 color, glm::vec3 position, float ambient, float constAtt, float linAtt, float expAtt);
};

#endif