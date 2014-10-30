#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <string>

#include "Shader.h"
#include <glm/glm.hpp>

class CDirectLight
{
public:
	glm::vec3 color;	//color of light
	glm::vec3 direction;

	float ambient;

	void setUniform(CShaderProgram* program, std::string lightVarName);

	CDirectLight();
	CDirectLight(glm::vec3 color, glm::vec3 direction, float ambient);
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

	void setUniform(CShaderProgram* program, std::string lightVarName);

	CPointLight();
	CPointLight(glm::vec3 color, glm::vec3 position, float ambient, float constAtt, float linAtt, float expAtt);
};

#endif