#ifndef _MODEL_H_
#define _MODEL_H_

#include <vector>

// Include AssImp 
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "hader.h"
#include "VBO.h"
#include "Texture.h"

class CMaterial
{
public:
	int Texture;
};

class CModel
{
public:
	CModel();
	CModel(char* file);

	bool load(char* file);
	//bool load_BumpMap(char* file, char* colorMap, char* normalMap);

	static void finalizeVBO();
	static void bindVAO();

	void render();
	//void render_BumpMap(CShaderProgram* program);

private:
	bool loaded;
	static CVBO vboModelData;
	static GLuint VAO;
	static std::vector<CTexture> textures;
	std::vector<int> meshStartIndices;
	std::vector<int> meshSize;
	std::vector<int> materialIndices;
	int numMaterials;
};

#endif