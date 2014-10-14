#include "Model.h"

CVBO CModel::vboModelData;

GLuint CModel::VAO;

std::vector<CTexture> CModel::textures;

string GetDirectoryPath(string sFilePath)
{
	// Get directory path
	string sDirectory = "";
	for (int i = sFilePath.size() - 1; i >= 0; i--)if (sFilePath[i] == '\\' || sFilePath[i] == '/')
	{
		sDirectory = sFilePath.substr(0, i + 1);
		break;
	}
	return sDirectory;
}

CModel::CModel()
{
	loaded = false;
}

CModel::CModel(char* file)
{
	loaded = false;
	load(file);
}

bool CModel::load(char* file)
{
	if (vboModelData.getID() == 0)
	{
		vboModelData.create();
		textures.reserve(50);
	}
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	if (!scene)
	{
		std::string errormsg = "Couldn't load model ";
		errormsg += file;
		MessageBox(NULL, errormsg.c_str(), "Error Importing Asset", MB_ICONERROR);
		return false;
	}

	const int vertexTotalSize = sizeof(aiVector3D)* 2 + sizeof(aiVector2D);

	int totalVertices = 0;

	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];
		int iMeshFaces = mesh->mNumFaces;
		materialIndices.push_back(mesh->mMaterialIndex);
		int iSizeBefore = vboModelData.getCurrentSize();
		meshStartIndices.push_back(iSizeBefore / vertexTotalSize);
		for (int j = 0; j < iMeshFaces; j++)
		{
			const aiFace& face = mesh->mFaces[j];
			for (int k = 0; k < 3; k++)
			{
				aiVector3D pos = mesh->mVertices[face.mIndices[k]];
				aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[k]];
				aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[face.mIndices[k]] : aiVector3D(1.0f, 1.0f, 1.0f);
				vboModelData.addData(&pos, sizeof(aiVector3D));
				vboModelData.addData(&uv, sizeof(aiVector2D));
				vboModelData.addData(&normal, sizeof(aiVector3D));
			}
		}
		int iMeshVertices = mesh->mNumVertices;
		totalVertices += iMeshVertices;
		meshSize.push_back((vboModelData.getCurrentSize() - iSizeBefore) / vertexTotalSize);
	}

	numMaterials = scene->mNumMaterials;

	vector<int> materialRemap(numMaterials);

	for (int i = 0; i < numMaterials; i++)
	{
		const aiMaterial* material = scene->mMaterials[i];
		int a = 5;
		int texIndex = 0;
		aiString path;  // filename

		if (material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path) == AI_SUCCESS)
		{
			string sDir = GetDirectoryPath(file);
			string sTextureName = path.data;
			string sFullPath = sDir + sTextureName;
			int iTexFound = -1;
			for (int j = 0; j < textures.size(); j++)if (sFullPath == textures[j].getFile())
			{
				iTexFound = j;
				break;
			}
			if (iTexFound != -1)materialRemap[i] = iTexFound;
			else
			{
				CTexture tNew;
				tNew.load_2D(sFullPath, true);
				materialRemap[i] = textures.size();
				textures.push_back(tNew);
			}
		}
	}

	for (int i = 0; i < meshSize.size(); i++)
	{
		int iOldIndex = materialIndices[i];
		materialIndices[i] = materialRemap[iOldIndex];
	}
	return loaded = true;
}

void CModel::finalizeVBO()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	vboModelData.bind();
	vboModelData.uploadGPU(GL_STATIC_DRAW);

	//Vertex position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(aiVector3D)+sizeof(aiVector2D), 0);
	//Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(aiVector3D)+sizeof(aiVector2D), (void*)sizeof(aiVector3D));
	//Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(aiVector3D)+sizeof(aiVector2D), (void*)(sizeof(aiVector3D)+sizeof(aiVector2D)));
}

void CModel::bindVAO()
{
	glBindVertexArray(VAO);
}

void CModel::render()
{
	if (!loaded)	return;
	int numMeshes = meshSize.size();
	for (int i = 0; i < numMeshes; i++)
	{
		int matIndex = materialIndices[i];
		textures[matIndex].bind();
		glDrawArrays(GL_TRIANGLES, meshStartIndices[i], meshSize[i]);
	}
}