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

CModel::CModel(std::string file)
{
	loaded = false;
	Load(file);
}

bool CModel::Load(std::string file)
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
		Error("Error importing asset", "Error loading model: " + file + "\nError code: " + string(importer.GetErrorString()));
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
				aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[face.mIndices[k]] : aiVector3D(1.0f, 1.0f, 1.0f);
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

	for (auto tex : textures) tex.SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);

	glGenVertexArrays(1, &VAO);

	loaded = true;
	return loaded;
}

void CModel::UploadVBO()
{
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

void CModel::Render(bool texture)
{
	if (!loaded)	return;
	int numMeshes = meshSize.size();
	for (int i = 0; i < numMeshes; i++)
	{
		if (texture)
		{
			int specIndex = materialIndices[i][1];
			if (specIndex >= 0) textures[specIndex].Bind(2);
			int diffIndex = materialIndices[i][0];
			if (diffIndex >= 0) textures[diffIndex].Bind();
		}

		glDrawArrays(GL_TRIANGLES, meshStartIndices[i], meshSize[i]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void CModel::ReleaseAll()
{
	vboModelData.Release();
	glDeleteVertexArrays(1, &VAO);
	for (auto tex : textures)	tex.Release();
}

//================================================================

CVBO CInstancedModel::vboMatrixData;

CInstancedModel::CInstancedModel()
{

}

void CInstancedModel::UploadMatrices(int count, glm::mat4* model)
{
	if (vboMatrixData.GetID() == 0)
	{
		vboMatrixData.Create();
	}

	Count = count;
	ModelMatrices = model;

	glBindVertexArray(VAO);

	
	vboMatrixData.AddData(model, count*sizeof(glm::mat4));
	

	vboMatrixData.Bind();
	vboMatrixData.UploadGPU(GL_STATIC_DRAW);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (GLvoid*)0);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (GLvoid*)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (GLvoid*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (GLvoid*)(3 * sizeof(glm::vec4)));

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	glBindVertexArray(0);
}

void CInstancedModel::RenderInstanced(int count, bool texture)
{
	if (!loaded)	return;
	int RenderCount = (count == 0) ? Count : count;
	int numMeshes = meshSize.size();
	for (int i = 0; i < numMeshes; i++)
	{
		if (texture)
		{
			int specIndex = materialIndices[i][1];
			if (specIndex >= 0) textures[specIndex].Bind(2);
			int diffIndex = materialIndices[i][0];
			if (diffIndex >= 0) textures[diffIndex].Bind();
		}

		glDrawArraysInstanced(GL_TRIANGLES, meshStartIndices[i], meshSize[i], RenderCount);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void CInstancedModel::ReleaseAllInstanced()
{
	vboMatrixData.Release();
}