#include "Light.h"

CDirectLight::CDirectLight()
{
	Color = glm::vec3(1.0f, 1.0f, 1.0f);
	Direction = glm::vec3(0.0f, -1.0f, 0.0f);

	Ambient = 0.25f;
}

CDirectLight::CDirectLight(glm::vec3 color, glm::vec3 direction, float ambient, float brightness)
{
	this->Color = color;
	this->Direction = direction;
	this->Ambient = ambient;
	this->Brightness = brightness;
}

void CDirectLight::SetUniform(CShaderProgram* program, std::string lightVarName)
{
	program->SetUniform(lightVarName + ".vColor", Color);
	program->SetUniform(lightVarName + ".vDirection", Direction);
	program->SetUniform(lightVarName + ".fAmbient", Ambient);
	program->SetUniform(lightVarName + ".fBrightness", Brightness);
}

//=============================================================================================

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

void CPointLight::SetUniform(CShaderProgram* program, std::string lightVarName)
{
	program->SetUniform(lightVarName + ".vColor", Color);
	program->SetUniform(lightVarName + ".vDirection", Position);
	program->SetUniform(lightVarName + ".fAmbient", Ambient);
	program->SetUniform(lightVarName + ".fConstantAtt", ConstantAttenuation);
	program->SetUniform(lightVarName + ".fLinearAtt", LinearAttenuation);
	program->SetUniform(lightVarName + "fExpAtt", ExponentialAttenuation);
}
