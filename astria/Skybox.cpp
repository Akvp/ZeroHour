#include "Common.h"

#include "Skybox.h"
#include "FileManager.h"

void CSkybox::Load(std::string a_sDirectory)
{
	std::vector<std::string> files = FileManager::GetFilesInFolder(a_sDirectory);
	
	for (auto f : files)
	{
		if (FileManager::GetFilenameWithoutExt(f) == "up")
		{
			sTop = f;
		}
		else if (FileManager::GetFilenameWithoutExt(f) == "dn")
		{
			sBottom = f;
		}
		else if (FileManager::GetFilenameWithoutExt(f) == "lf")
		{
			sLeft = f;
		}
		else if (FileManager::GetFilenameWithoutExt(f) == "rt")
		{
			sRight = f;
		}
		else if (FileManager::GetFilenameWithoutExt(f) == "ft")
		{
			sFront = f;
		}
		else if (FileManager::GetFilenameWithoutExt(f) == "bk")
		{
			sBack = f;
		}
	}

	tTextures[0].Load_2D(sFront);
	tTextures[1].Load_2D(sBack);
	tTextures[2].Load_2D(sLeft);
	tTextures[3].Load_2D(sRight);
	tTextures[4].Load_2D(sTop);
	tTextures[5].Load_2D(sBottom);

	sDirectory = a_sDirectory;

	for (int i = 0; i < 6; i++)
	{
		tTextures[i].SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR);
		tTextures[i].SetSamplerParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		tTextures[i].SetSamplerParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glGenVertexArrays(1, &uiVAO);
	glBindVertexArray(uiVAO);

	vboRenderData.Create();
	vboRenderData.Bind();

	//All vertices
	//8 vertices of a cube, repeated 3 times each
	glm::vec3 vSkyBoxVertices[24] = 
	{
		// Front face
		glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(-1.0f, -1.0f, 1.0f),
		// Back face
		glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, -1.0f, -1.0f),
		// Left face
		glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, -1.0f),
		// Right face
		glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, -1.0f, 1.0f),
		// Top face
		glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f),
		// Bottom face
		glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(-1.0f, -1.0f, 1.0f),
	};

	//Texture coordinates
	glm::vec2 vSkyBoxTexCoords[4] =
	{ 
		//glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)
		glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f)
	};

	//Normal for each faces
	glm::vec3 vSkyBoxNormals[6] = 
	{
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	};

	for (int i = 0; i < 24; i++)
	{
		vboRenderData.AddData(&vSkyBoxVertices[i], sizeof(glm::vec3));
		vboRenderData.AddData(&vSkyBoxTexCoords[i%4], sizeof(glm::vec2));
		vboRenderData.AddData(&vSkyBoxNormals[i/4], sizeof(glm::vec3));
	}

	vboRenderData.UploadGPU(GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3)+sizeof(glm::vec2), 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3)+sizeof(glm::vec2), (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3)+sizeof(glm::vec2), (void*)(sizeof(glm::vec3)+sizeof(glm::vec2)));
}

void CSkybox::Render()
{
	glDepthMask(0);
	glBindVertexArray(uiVAO);
	for (int i = 0; i < 6; i++)
	{
		tTextures[i].Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, i*4, 4);
	}
	glDepthMask(1);
}

void CSkybox::Release()
{
	for(int i = 0; i < 6; i++)tTextures[i].Release();
	glDeleteVertexArrays(1, &uiVAO);
	vboRenderData.Release();
}