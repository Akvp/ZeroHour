#ifndef _HEIGHTMAP_H_
#define _HEIGHTMAP_H_

#include "Common.h"
#include "Shader.h"
#include "VBO.h"

#define NUM_TERRAIN_SHADER 10

class CHeightMap
{
public:
	CHeightMap();

	static bool LoadShaderProgram();
	static bool ReleaseShaderProgram();

	bool Load(std::string file);
	void Release();

	void Render();

	void SetRenderSize(float QuadSize, float Height);
	void SetRenderSize(float x, float h, float z);

	int GetRows();
	int GetCols();

	static CShaderProgram* GetShaderProgram();

private:
	UINT vao;
	bool loaded;
	bool programLoaded;
	int rows;
	int cols;

	glm::vec3 RenderScale;
	
	CVBO HeightMapData;
	CVBO HeightMapIndices;

	static CShaderProgram Terrain;
	static CShader TerrainShaders[NUM_TERRAIN_SHADER];
};

#endif