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
	bool Create();

	//TODO: implement shadow mapping for point light and spotlight
	CFBO* Render(void* RenderCallBack(), CDirectLight* sun);
	bool LoadShaders(std::string);
	void SetProgram(CShaderProgram* mapping, CShaderProgram* rendering);
private:
	CFBO FBOShadowMap;
	CVBO VBOShadowMap;
	GLuint VAOShadowMap;
	int ShadowMapSizeTextureSize;

	CShaderProgram* ProgramMap;
	CShaderProgram* ProgramRender;

};

#endif