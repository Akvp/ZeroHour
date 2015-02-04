#ifndef _FOG_H_
#define _FOG_H_

#include <cstdarg>
#include "Common.h"
#include "Shader.h"

class CFog
{
public:
	static void SetUniforms(CShaderProgram* program, int enabled = 1);
	static void SetUniforms(int n_arg, ...);

	static void Properties(float density, glm::vec4 color);

private:
	static float Density;
	static glm::vec4 Color;
};

#endif