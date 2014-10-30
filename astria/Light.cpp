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

CPointLight::CPointLight()
{
	Color = glm::vec3(1.0f, 1.0f, 1.0f);
	Position = glm::vec3(0.0f, 0.0f, 0.0f);
	Ambient = 0.35f;
	ConstantAttenuation = 0.3f;
	LinearAttenuation = 0.007f;
	ExponentialAttenuation = 0.00008f;
}

CPointLight::CPointLight(glm::vec3 color, glm::vec3 position, float ambient, float constAtt, float linAtt, float expAtt)
{
	Color = color;
	Position = position;
	Ambient = ambient;
	ConstantAttenuation = constAtt;
	LinearAttenuation = linAtt;
	ExponentialAttenuation = expAtt;
}

void CPointLight::setUniform(CShaderProgram* program, std::string lightVarName)
{
	program->SetUniform(lightVarName + ".vColor", Color);
	program->SetUniform(lightVarName + ".vDirection", Position);
	program->SetUniform(lightVarName + ".fAmbient", Ambient);
	program->SetUniform(lightVarName + ".fConstantAtt", ConstantAttenuation);
	program->SetUniform(lightVarName + ".fLinearAtt", LinearAttenuation);
	program->SetUniform(lightVarName + "fExpAtt", ExponentialAttenuation);
}