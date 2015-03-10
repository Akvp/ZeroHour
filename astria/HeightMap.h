#ifndef _HEIGHTMAP_H_
#define _HEIGHTMAP_H_

#include "Common.h"
#include "Shader.h"
#include "VBO.h"

class CHeightMap
{
public:
	CHeightMap();

	static bool LoadShaderProgram(string vertex, string fragment);
	static void ReleaseShaderProgram();
	static CShaderProgram* GetShaderProgram();

	bool Load(std::string file);
	void Release();

	void Render();

	void SetSize(float QuadSize, float Height);
	void SetSize(float x, float h, float z);

	int GetRows();
	int GetCols();

	float GetHeight(glm::vec3 position);

private:
	UINT vao;
	bool loaded;
	bool programLoaded;
	int rows;
	int cols;

	glm::vec3 RenderScale;
	glm::mat4 RenderScaleMatrix;
	glm::mat3 NormalScaleMatrix;

	vector<vector<glm::vec3>> VertexData;
	
	CVBO HeightMapData;
	CVBO HeightMapIndices;

	static CShaderProgram ProgramTerrain;
	static CShader ShaderVertex;
	static CShader ShaderFragment;
};

#endif