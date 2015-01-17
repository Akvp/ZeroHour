#ifndef _HEIGHTMAP_H_
#define _HEIGHTMAP_H_

#include "Common.h"
#include "Shader.h"
#include "VBO.h"
#include "utils.h"

#define NUM_TERRAIN_SHADER 3

class CHeightMap
{
public:
	CHeightMap();

	static bool LoadShaderProgram();
	static void ReleaseShaderProgram();
	static CShaderProgram* GetShaderProgram();

	bool Load(std::string file);
	void Release();

	void Render();

	void SetRenderSize(float QuadSize, float Height);
	void SetRenderSize(float x, float h, float z);

	int GetRows();
	int GetCols();

private:
	UINT vao;
	bool loaded;
	bool programLoaded;
	int rows;
	int cols;

	glm::vec3 RenderScale;

	vector<vector<glm::vec3>> VertexData;
	
	CVBO HeightMapData;
	CVBO HeightMapIndices;

	static CShaderProgram Program_Terrain;
	static CShader Shader_Terrain[NUM_TERRAIN_SHADER];
};

#endif