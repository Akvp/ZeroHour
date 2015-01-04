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
	Load(file);
}

bool CModel::Load(char* file)
{
	if (vboModelData.GetID() == 0)
	{
		vboModelData.Create();
		textures.reserve(50);
	}
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_FlipUVs);

	if (!scene)
	{
		string errormsg = "Couldn't load model: ";
		errormsg += file;
		errormsg = errormsg + "\nError code:" + string(importer.GetErrorString());
		MessageBox(NULL, errormsg.c_str(), "Error Importing Asset", MB_ICONERROR);
		return false;
	}

	const int vertexTotalSize = sizeof(aiVector3D)* 2 + sizeof(aiVector2D);

	int totalVertices = 0;

	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];
		int iMeshFaces = mesh->mNumFaces;

		vector<int> tmpIndex;
		tmpIndex.push_back(mesh->mMaterialIndex);
		tmpIndex.push_back(mesh->mMaterialIndex);
		materialIndices.push_back(tmpIndex);

		int iSizeBefore = vboModelData.GetCurrentSize();
		meshStartIndices.push_back(iSizeBefore / vertexTotalSize);
		for (int j = 0; j < iMeshFaces; j++)
		{
			const aiFace& face = mesh->mFaces[j];
			for (int k = 0; k < 3; k++)
			{
				aiVector3D pos = mesh->mVertices[face.mIndices[k]];
				aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[k]];
				aiVector3D normal = mesh->mNormals[face.mIndices[k]];
				vboModelData.AddData(&pos, sizeof(aiVector3D));
				vboModelData.AddData(&uv, sizeof(aiVector2D));
				vboModelData.AddData(&normal, sizeof(aiVector3D));
			}
		}

		int iMeshVertices = mesh->mNumVertices;
		totalVertices += iMeshVertices;
		meshSize.push_back((vboModelData.GetCurrentSize() - iSizeBefore) / vertexTotalSize);
	}

	numMaterials = scene->mNumMaterials;

	vector<int> diffuseRemap(numMaterials); diffuseRemap = { -1, };
	vector<int> specularRemap(numMaterials); specularRemap = { -1, };

	for (int i = 0; i < numMaterials; i++)
	{
		const aiMaterial* material = scene->mMaterials[i];
		int texIndex = 0;
		aiString path;  // filename

		if (material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path) == AI_SUCCESS)
		{
			string sDir = GetDirectoryPath(file);
			string sTextureName = path.data;
			string sFullPath = sDir + sTextureName;
			int iTexFound = -1;
			for (int j = 0; j < textures.size(); j++)
			{
				if (sFullPath == textures[j].GetFile())
				{
					iTexFound = j;
					break;
				}
			}
			if (iTexFound != -1)
			{
				diffuseRemap[i] = iTexFound;
			}
			else
			{
				CTexture tNew;
				tNew.Load_2D(sFullPath, true);
				diffuseRemap[i] = textures.size();
				textures.push_back(tNew);
			}
		}
		if (material->GetTexture(aiTextureType_SPECULAR, texIndex, &path) == AI_SUCCESS)
		{
			string sDir = GetDirectoryPath(file);
			string sTextureName = path.data;
			string sFullPath = sDir + sTextureName;
			int iTexFound = -1;
			for (int j = 0; j < textures.size(); j++)
			{
				if (sFullPath == textures[j].GetFile())
				{
					iTexFound = j;
					break;
				}
			}
			if (iTexFound != -1)
			{
				specularRemap[i] = iTexFound;
			}
			else
			{
				CTexture tNew;
				tNew.Load_2D(sFullPath, true);
				specularRemap[i] = textures.size();
				textures.push_back(tNew);
			}
		}
	}
	for (int i = 0; i < meshSize.size(); i++)
	{
		materialIndices[i][0] = diffuseRemap[materialIndices[i][0]];
		materialIndices[i][1] = specularRemap[materialIndices[i][1]];
	}

	loaded = true;
	return loaded;
}

void CModel::UploadVBO()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	vboModelData.Bind();
	vboModelData.UploadGPU(GL_STATIC_DRAW);

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

void CModel::BindVAO()
{
	glBindVertexArray(VAO);
}

void CModel::Render()
{
	if (!loaded)	return;
	int numMeshes = meshSize.size();
	for (int i = 0; i < numMeshes; i++)
	{
		int specIndex = materialIndices[i][1];
		if (specIndex >= 0) textures[specIndex].Bind(2);
		int diffIndex = materialIndices[i][0];
		textures[diffIndex].Bind();
		glDrawArrays(GL_TRIANGLES, meshStartIndices[i], meshSize[i]);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void CModel::Release()
{
	vboModelData.Release();
	glDeleteVertexArrays(1, &VAO);
	for (auto tex : textures)	tex.Release();
}