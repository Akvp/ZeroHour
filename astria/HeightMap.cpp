#include "HeightMap.h"

CShaderProgram CHeightMap::Terrain;
CShader CHeightMap::TerrainShaders[NUM_TERRAIN_SHADER];

CHeightMap::CHeightMap()
{
	RenderScale = glm::vec3(1.0f, 1.0f, 1.0f);
}

bool CHeightMap::Load(std::string file)
{
	if (loaded)
	{
		loaded = false;
		Release();
	}

	SDL_Surface* Surf_Load = IMG_Load(file.c_str());
	
	if (Surf_Load == NULL)
	{
		char errorMsg[512] = "";
		sprintf(errorMsg, "Error loading height map %s\n Error message: %s\n", file, IMG_GetError());
		MessageBox(NULL, errorMsg, "Height Map Loading Error", MB_ICONERROR);
		return false;
	}

	//3 bytes per pixels
	//RGB texture
	if (Surf_Load->format->BytesPerPixel == 3)
	{

	}
	//1 byte per pixel
	//Depth texture
	else if (Surf_Load->format->BytesPerPixel == 1)
	{

	}

	return true;
}

void CHeightMap::Release()
{

}

bool CHeightMap::LoadShaderProgram()
{
	bool ret = true;
	ret = ret & TerrainShaders[0].load("shaders/terrain.vert", GL_VERTEX_SHADER);
	ret = ret & TerrainShaders[1].load("shaders/terrain.frag", GL_FRAGMENT_SHADER);
	ret = ret & TerrainShaders[2].load("shaders/dirLight.frag", GL_FRAGMENT_SHADER);

	Terrain.create();
	for (int i = 0; i < NUM_TERRAIN_SHADER; i++)	Terrain.addShader(&TerrainShaders[i]);
	Terrain.link();

	return ret;
}

void CHeightMap::SetRenderSize(float x, float h, float z)
{
	RenderScale = glm::vec3(x, h, z);
}

void CHeightMap::SetRenderSize(float QuadSize, float Height)
{
	RenderScale = glm::vec3(float(cols)*QuadSize, Height, float(rows)*QuadSize);
}

int CHeightMap::GetRows()
{
	return rows;
}

int CHeightMap::GetCols()
{
	return cols;
}

void CHeightMap::Render()
{
	Terrain.use();

	Terrain.SetUniform("fRenderHeight", RenderScale.y);
	Terrain.SetUniform("fMaxTextureU", float(cols)*0.1f);
	Terrain.SetUniform("fMaxTextureV", float(rows)*0.1f);

	Terrain.SetUniform("HeightmapScaleMatrix", glm::scale(glm::mat4(1.0), glm::vec3(RenderScale)));

	glBindVertexArray(vao);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(rows*cols);

	int NumIndices = (rows - 1) * cols * 2 + rows - 1;
	glDrawElements(GL_TRIANGLE_STRIP, NumIndices, GL_UNSIGNED_INT, 0);
}