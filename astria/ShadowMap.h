#ifndef _SHADOW_MAP_H_
#define _SHADOW_MAP_H_

#include "Common.h"
#include "FBO.h"
#include "VBO.h"
#include "Shader.h"
#include "Light.h"

class CShadowMap
{
public:
	CShadowMap::CShadowMap();
	bool Create(int mapSize);

	//TODO: implement shadow mapping for point light and spotlight
	//returns DepthBiasMVP
	glm::mat4 Render(void (*RenderCallBack)(CShaderProgram* program, glm::mat4 projection, glm::mat4 view), CDirectLight* sun);
	
	//Return the fbo for the shadow map
	CFBO* GetFBO();

	//Load the shaders and programs
	bool LoadShaders(std::string map_vert, std::string map_frag);
	//Get shadow map programs
	CShaderProgram* GetProgram();

private:
	CFBO FBOShadowMap;
	CVBO VBOShadowMap;
	GLuint VAOShadowMap;
	int ShadowMapSizeTextureSize;

	CShader ShaderVertex;
	CShader ShaderFragment;

	CShaderProgram* ProgramMap;
};

#endif