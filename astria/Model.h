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

class CModel
{
public:
	CModel();
	CModel(std::string file);

	bool Load(std::string file);

	static void UploadVBO();
	static void BindVAO();

	void Render(bool texture = true);

	//Release all model
	static void ReleaseAll();

protected:
	bool loaded;
	static CVBO vboModelData;
	static GLuint VAO;
	static vector<CTexture> textures;
	vector<int> meshStartIndices;
	vector<int> meshSize;
	vector<vector<int>> materialIndices;	
	int numMaterials;
};

class CInstancedModel : public CModel
{
public:
	CInstancedModel();

	void UploadMatrices(int count, glm::mat4* model);
	//If count is 0, render as many as Model matrices available
	void RenderInstanced(int count = 0, bool texture = true);

	static void ReleaseAllInstanced();
private:
	int Count;
	static CVBO vboMatrixData;
	glm::mat4* ModelMatrices;
};

#endif