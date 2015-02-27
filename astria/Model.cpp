 #include "Model.h"

CVBO CModel::vboModelData;
CVBO CModel::vboTangentData;

GLuint CModel::VAO;

vector<CTexture> CModel::TextureDiffuse;
vector<CTexture> CModel::TextureNormal;
vector<CTexture> CModel::TextureSpecular;

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
	Loaded = false;
	bNormalMap = false;
}

CModel::CModel(std::string file)
{
	Loaded = false;
	bNormalMap = false;
	Load(file);
}

bool CModel::Load(std::string file)
{
	if (vboModelData.GetID() == 0)
	{
		vboModelData.Create();
		vboTangentData.Create();
		TextureDiffuse.reserve(50);
		TextureNormal.reserve(50);
		TextureSpecular.reserve(50);
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

		DiffuseIndices.push_back(mesh->mMaterialIndex);
		NormalIndices.push_back(mesh->mMaterialIndex);
		SpecularIndices.push_back(mesh->mMaterialIndex);

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
				if (mesh->HasTangentsAndBitangents())
				{
					bNormalMap = true;
					aiVector3D tangent = mesh->mTangents[face.mIndices[k]];
					aiVector3D bitangent = mesh->mBitangents[face.mIndices[k]];
					vboTangentData.AddData(&tangent, sizeof(aiVector3D));
					vboTangentData.AddData(&bitangent, sizeof(aiVector3D));
				}
			}
		}

		int iMeshVertices = mesh->mNumVertices;
		totalVertices += iMeshVertices;
		meshSize.push_back((vboModelData.GetCurrentSize() - iSizeBefore) / vertexTotalSize);
	}

	numMaterials = scene->mNumMaterials;

	vector<int> diffuseRemap(numMaterials); diffuseRemap.assign(numMaterials, -1);
	vector<int> normalRemap(numMaterials); normalRemap.assign(numMaterials, -1);
	vector<int> specularRemap(numMaterials); specularRemap.assign(numMaterials, -1);

	for (int i = 0; i < numMaterials; i++)
	{
		const aiMaterial* material = scene->mMaterials[i];
		int texIndex = 0;
		aiString path;  // filename
		
		for (texIndex = 0; texIndex < material->GetTextureCount(aiTextureType_DIFFUSE); texIndex++)
		{
			if (material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path) == AI_SUCCESS)
			{
				string sDir = GetDirectoryPath(file);
				string sTextureName = path.data;
				string sFullPath = sDir + sTextureName;
				int iTexFound = -1;
				for (int j = 0; j < TextureDiffuse.size(); j++)
				{
					if (sFullPath == TextureDiffuse[j].GetFile())
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
					diffuseRemap[i] = TextureDiffuse.size();
					TextureDiffuse.push_back(tNew);
				}
			}
		}
		for (texIndex = 0; texIndex < material->GetTextureCount(aiTextureType_SPECULAR); texIndex++)
		{
			if (material->GetTexture(aiTextureType_SPECULAR, texIndex, &path) == AI_SUCCESS)
			{
				string sDir = GetDirectoryPath(file);
				string sTextureName = path.data;
				string sFullPath = sDir + sTextureName;
				int iTexFound = -1;
				for (int j = 0; j < TextureSpecular.size(); j++)
				{
					if (sFullPath == TextureSpecular[j].GetFile())
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
					specularRemap[i] = TextureSpecular.size();
					TextureSpecular.push_back(tNew);
				}
			}
		}
		printf("Normal maps count: %d\n", material->GetTextureCount(aiTextureType_HEIGHT));
		for (texIndex = 0; texIndex < material->GetTextureCount(aiTextureType_HEIGHT); texIndex++)
		{
			if (material->GetTexture(aiTextureType_HEIGHT, texIndex, &path) == AI_SUCCESS)
			{
				string sDir = GetDirectoryPath(file);
				string sTextureName = path.data;
				string sFullPath = sDir + sTextureName;
				int iTexFound = -1;
				for (int j = 0; j < TextureNormal.size(); j++)
				{
					if (sFullPath == TextureNormal[j].GetFile())
					{
						iTexFound = j;
						break;
					}
				}
				if (iTexFound != -1)
				{
					normalRemap[i] = iTexFound;
				}
				else
				{
					CTexture tNew;
					tNew.Load_2D(sFullPath, true);
					normalRemap[i] = TextureNormal.size();
					TextureNormal.push_back(tNew);
				}
			}
		}
	}
	for (int i = 0; i < meshSize.size(); i++)
	{
		DiffuseIndices[i] = diffuseRemap[DiffuseIndices[i]];
		NormalIndices[i] = normalRemap[NormalIndices[i]];
		SpecularIndices[i] = specularRemap[SpecularIndices[i]];
	}

	for (auto tex : TextureDiffuse) tex.SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
	for (auto tex : TextureNormal) tex.SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
	for (auto tex : TextureSpecular) tex.SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);

	glGenVertexArrays(1, &VAO);

	Loaded = true;
	return Loaded;
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

	vboTangentData.Bind();
	vboTangentData.UploadGPU(GL_STATIC_DRAW);

	// Tangent vector
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(aiVector3D), 0);
	// Bitangent vector
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(aiVector3D), (void*)(sizeof(aiVector3D)));
}

void CModel::BindVAO()
{
	glBindVertexArray(VAO);
}

void CModel::Render(bool texture)
{
	if (!Loaded)	return;
	int numMeshes = meshSize.size();
	for (int i = 0; i < numMeshes; i++)
	{
		if (texture)
		{
			int specIndex = SpecularIndices[i];
			if (specIndex >= 0) 
				TextureSpecular[specIndex].Bind(2);

			int normIndex = NormalIndices[i];
			if (normIndex >= 0)
				TextureNormal[normIndex].Bind(1);

			int diffIndex = DiffuseIndices[i];
			if (diffIndex >= 0) 
				TextureDiffuse[diffIndex].Bind();
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
	for (auto tex : TextureDiffuse)		tex.Release();
	for (auto tex : TextureNormal)		tex.Release();
	for (auto tex : TextureSpecular)	tex.Release();
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
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (GLvoid*)0);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (GLvoid*)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (GLvoid*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (GLvoid*)(3 * sizeof(glm::vec4)));

	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);
	glVertexAttribDivisor(8, 1);

	glBindVertexArray(0);
}

void CInstancedModel::RenderInstanced(int count, bool texture)
{
	if (!Loaded)	return;
	int RenderCount = (count == 0) ? Count : count;
	int numMeshes = meshSize.size();
	for (int i = 0; i < numMeshes; i++)
	{
		if (texture)
		{
			int specIndex = SpecularIndices[i];
			if (specIndex >= 0) TextureSpecular[specIndex].Bind(2);
			int diffIndex = DiffuseIndices[i];
			if (diffIndex >= 0) TextureDiffuse[diffIndex].Bind();
		}

		glDrawArraysInstanced(GL_TRIANGLES, meshStartIndices[i], meshSize[i], RenderCount);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void CInstancedModel::ReleaseAllInstanced()
{
	vboMatrixData.Release();
}