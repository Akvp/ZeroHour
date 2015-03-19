#include "Fog.h"

float CFog::Density;
glm::vec4 CFog::Color;

void CFog::SetUniforms(CShaderProgram* program, int enabled)
{
	program->SetUniform("bFog", enabled);
	program->SetUniform("fogParams.iEquation", 2);
	program->SetUniform("fogParams.vFogColor", Color);
	program->SetUniform("fogParams.fDensity", Density);
}

void CFog::SetUniforms(int n_arg, ...)
{
	va_list programs;
	va_start(programs, n_arg);
	for (int i = 0; i < n_arg; i++)
	{
		CShaderProgram* program = va_arg(programs, CShaderProgram*);
		program->SetUniform("bFog", 1);
		program->SetUniform("fogParams.iEquation", 1);
		program->SetUniform("fogParams.vFogColor", Color);
		program->SetUniform("fogParams.fDensity", Density);
	}
	va_end(programs);
}

void CFog::Properties(float density, glm::vec4 color)
{
	Density = density;
	Color = color;
}