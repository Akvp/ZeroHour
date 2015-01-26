#include "Water.h"

CShader CWaterPlane::ShaderVertex;
CShader CWaterPlane::ShaderFragment;
CShaderProgram CWaterPlane::ProgramMain;

CWaterPlane::CWaterPlane()
{

}

bool CWaterPlane::Load(string normal, string derivative, glm::vec3 position, float sizex, float sizez)
{
	glm::vec2 UV[4] = { glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1, 1), glm::vec2(0, 1) };
	glm::vec3 p0 = position; p0.x += sizex;
	glm::vec3 p1 = position; p1.z += sizez;
	glm::vec3 p2 = position; p2.x += sizex; p2.z += sizez;
	glm::vec3 Plane[4] = { position, p0, p1, p2 };
	VBO.Create();
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	VBO.Bind();

	for (int i = 0; i < 4; i++)
	{
		VBO.AddData(&Plane[i], sizeof(glm::vec3));
		glm::vec2 coord = UV[i];
		VBO.AddData(&coord, sizeof(glm::vec2));
	}

	VBO.UploadGPU(GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) + sizeof(glm::vec2), (void*)sizeof(glm::vec3));

	TextureNormal.Load_2D(normal, true);
	TextureNormal.SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
	TextureDerivative.Load_2D(derivative, true);
	TextureDerivative.SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
	return true;
}

void CWaterPlane::Render()
{
	ProgramMain.Use();
	ProgramMain.SetUniform("normal", 0);
	ProgramMain.SetUniform("derivative", 1);

	TextureNormal.Bind(0);
	TextureDerivative.Bind(1);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void CWaterPlane::Release()
{
	glDeleteVertexArrays(1, &VAO);
	VBO.Release();
}

bool CWaterPlane::LoadProgram(string vertex, string fragment)
{
	bool load = true;
	load && ShaderVertex.Load(vertex, GL_VERTEX_SHADER);
	load && ShaderFragment.Load(fragment, GL_FRAGMENT_SHADER);
	load && ProgramMain.Initiate(&ShaderVertex, &ShaderFragment);
	return load;
}

void CWaterPlane::ReleaseProgram()
{
	ProgramMain.Release();
	ShaderVertex.Release();
	ShaderFragment.Release();
}

CShaderProgram* CWaterPlane::GetProgram()
{
	return &ProgramMain;
}