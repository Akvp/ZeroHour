#ifndef _MODEL_H_
#define _MODEL_H_

#include <vector>

// Include AssImp 
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "Shader.h"
#include "VBO.h"
#include "Texture.h"
#include "utils.h"

class CMesh
{
public:
	void Draw();

private:
	static CVBO vboMeshData;
	static GLuint VAO;
};

class CModel
{
public:
	CModel();
	CModel(char* file);

	bool Load(char* file);

	static void UploadVBO();
	static void BindVAO();

	void Render();

	//Release all model
	static void Release();

private:
	bool loaded;
	static CVBO vboModelData;
	static GLuint VAO;
	static vector<CTexture> textures;
	vector<int> meshStartIndices;
	vector<int> meshSize;
	vector<vector<int>> materialIndices;	
	int numMaterials;
};

#endif