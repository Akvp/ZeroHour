#include "Light.h"

CDirectLight::CDirectLight()
{
	color = glm::vec3(1.0f, 1.0f, 1.0f);
	direction = glm::vec3(0.0f, -1.0f, 0.0f);

	ambient = 0.25f;
}

CDirectLight::CDirectLight(glm::vec3 color, glm::vec3 direction, float ambient)
{
	this->color = color;
	this->direction = direction;
	this->ambient = ambient;
}

void CDirectLight::setUniform(CShaderProgram* program, std::string lightVarName)
{
	program->SetUniform(lightVarName + ".vColor", color);
	program->SetUniform(lightVarName + ".vDirection", direction);
	program->SetUniform(lightVarName + ".fAmbient", ambient);
}