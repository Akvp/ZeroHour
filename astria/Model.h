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

	bool NormalMap() { return bNormalMap; }

	//Release all model
	static void ReleaseAll();

protected:
	bool Loaded;
	bool bNormalMap;
	static CVBO vboModelData;
	static CVBO vboTangentData;
	static GLuint VAO;
	static vector<CTexture> TextureDiffuse;
	static vector<CTexture> TextureNormal;
	static vector<CTexture> TextureSpecular;
	vector<int> meshStartIndices;
	vector<int> meshSize;

	vector<int> DiffuseIndices;
	vector<int> NormalIndices;
	vector<int> SpecularIndices;
	int numMaterials;
};

//For now, i only implemented instancing with varying model matrix
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