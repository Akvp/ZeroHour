#include "ShadowMap.h"

CShadowMap::CShadowMap()
{

}

bool CShadowMap::Create()
{
	glm::vec3 ShadowMapQuad[] =
	{
		glm::vec3(0.75f, 1.0f, 0.0f),
		glm::vec3(0.75f, 0.5f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 0.5f, 0.0f)
	};
	glm::vec2 ShadowMapQuadUV[] =
	{
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f)
	};

	VBOShadowMap.Create();
	for (int i = 0; i < 4; i++)
	{
		VBOShadowMap.AddData(&ShadowMapQuad[i], sizeof(glm::vec3));
		VBOShadowMap.AddData(&ShadowMapQuadUV[i], sizeof(glm::vec2));
	}

	glGenVertexArrays(1, &VAOShadowMap);
	glBindVertexArray(VAOShadowMap);

	VBOShadowMap.Bind();
	VBOShadowMap.UploadGPU(GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), (void*)sizeof(glm::vec3));

	FBOShadowMap.CreateWithTexture(ShadowMapSizeTextureSize, ShadowMapSizeTextureSize);
	FBOShadowMap.AddDepthBuffer();
	FBOShadowMap.SetTextureFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_NEAREST);

	return true;
}

CFBO* CShadowMap::Render(void* RenderCallBack(), CDirectLight* sun)
{
	//Render the scene from the light's POV
	FBOShadowMap.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ProgramMap->Use();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	ProgramMap->Use();

	const float RangeX = 150, RangeY = 150, MinZ = 0.05f, MaxZ = 400;
	glm::mat4 Projection = glm::ortho<float>(-RangeX, RangeX, RangeY, -RangeY, MinZ, MaxZ);
	glm::vec3 LightPosition = -sun->Direction*1000.0f;	//Since we have directional, we just make the light very far so it can see everything
	glm::mat4 View_Light = glm::lookAt(LightPosition, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

	glm::mat4 BiasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
		);

	//Calculate depth bias matrix
	glm::mat4 DepthBiasMVP = BiasMatrix * Projection * View_Light;

	RenderCallBack();
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return &FBOShadowMap;
}

void CShadowMap::SetProgram(CShaderProgram* mapping, CShaderProgram* rendering)
{
	ProgramMap = mapping;
	ProgramRender = rendering;
}