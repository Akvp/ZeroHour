#include "HeightMap.h"

CShaderProgram CHeightMap::Program_Terrain;
CShader CHeightMap::Shader_Vertex;
CShader CHeightMap::Shader_Fragment;

CHeightMap::CHeightMap()
{
	RenderScale = glm::vec3(1.0f, 1.0f, 1.0f);
}

bool CHeightMap::Load(std::string file)
{
	if (loaded)
	{
		loaded = false;
		Release();
	}

	SDL_Surface* Surf_Load = IMG_Load(file.c_str());
	
	if (Surf_Load == NULL)
	{
		Error("Height Map Loading Error", "Error loading heightmap: " + file + "\nError message: " + IMG_GetError());
		return false;
	}

	//This should work :S
	BYTE* DataPtr = (BYTE*)Surf_Load->pixels;
	rows = Surf_Load->h;
	cols = Surf_Load->w;

	//Return false if the image is not in RGB or Luminance
	if (DataPtr == NULL || rows == 0 || cols == 0 || (Surf_Load->format->BytesPerPixel != 3 && Surf_Load->format->BytesPerPixel != 1))
	{
		Error("Height Map Loading Error", "Error loading heightmap: " + file + "\nIncorrect image format\n");
		return false;
	}

	//Pointer increment depending of bytes per pixel
	unsigned int ptr_inc = Surf_Load->format->BytesPerPixel;
	//Length of one row
	unsigned int row_step = ptr_inc*cols;

	VertexData = std::vector<std::vector<glm::vec3>>(rows, std::vector<glm::vec3>(cols));
	std::vector<std::vector<glm::vec2>> CoordsData(rows, std::vector<glm::vec2>(cols));

	float textureU = float(cols)*0.1f;
	float textureV = float(rows)*0.1f;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			float scaleC = float(j) / float(cols - 1);
			float scaleR = float(i) / float(rows - 1);
			float VertexHeight = float(*(DataPtr + row_step*i + j*ptr_inc)) / 255.0f;
			VertexData[i][j] = glm::vec3(-0.5f + scaleC, VertexHeight, -0.5f + scaleR);
			CoordsData[i][j] = glm::vec2(textureU*scaleC, textureV*scaleR);
		}
	}

	//We have (rows-1)(cols-1) quads, each composed of 2 triangle
	std::vector<std::vector<glm::vec3>> Normals[2];
	for (int i = 0; i < 2; i++)
	{
		Normals[i] = std::vector<std::vector<glm::vec3>>(rows - 1, std::vector<glm::vec3>(cols - 1));
	}

	for (int i = 0; i < rows - 1; i++)
	{
		for (int j = 0; j < cols - 1; j++)
		{
			glm::vec3 Triangle0[] =
			{
				VertexData[i][j],
				VertexData[i + 1][j],
				VertexData[i + 1][j + 1]
			};
			glm::vec3 Triangle1[] =
			{
				VertexData[i + 1][j + 1],
				VertexData[i][j + 1],
				VertexData[i][j]
			};

			glm::vec3 TriangleNorm0 = glm::cross(Triangle0[0] - Triangle0[1], Triangle0[1] - Triangle0[2]);
			glm::vec3 TriangleNorm1 = glm::cross(Triangle1[0] - Triangle1[1], Triangle1[1] - Triangle1[2]);

			Normals[0][i][j] = glm::normalize(TriangleNorm0);
			Normals[1][i][j] = glm::normalize(TriangleNorm1);
		}
	}

	std::vector<std::vector<glm::vec3>> FinalNormals = std::vector<std::vector<glm::vec3>>(rows, std::vector<glm::vec3>(cols));

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			//calculate the final normal for each vertex

			glm::vec3 FinalNormal = glm::vec3(0.0f, 0.0f, 0.0f);

			//Upper-left triangles
			if (j != 0 && i != 0)
			{
				for (int k = 0; k < 2; k++)
				{
					FinalNormal += Normals[k][i - 1][j - 1];
				}
			}
			//Upper-right triangle
			if (i != 0 && j != cols - 1)
			{
				FinalNormal += Normals[0][i - 1][j];
			}
			//Lower-right triangles
			if (i != rows - 1 && j != cols - 1)
			{
				for (int k = 0; k < 2; k++)
				{
					FinalNormal += Normals[k][i][j];
				}
			}
			//Lower-left triangle
			if (i != rows - 1 && j != 0)
			{
				FinalNormal += Normals[1][i][j - 1];
			}
			FinalNormal = glm::normalize(FinalNormal);
			FinalNormals[i][j] = FinalNormal;
		}
	}

	//Create VBO with only vertex data
	HeightMapData.Create(rows*cols*(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			//Pretty self-explanatory here
			HeightMapData.AddData(&VertexData[i][j], sizeof(glm::vec3));
			HeightMapData.AddData(&CoordsData[i][j], sizeof(glm::vec2));
			HeightMapData.AddData(&FinalNormals[i][j], sizeof(glm::vec3));
		}
	}

	//Create VBO with height map indices
	HeightMapIndices.Create();
	int PrimitiveRestartIndex = rows*cols;
	for (int i = 0; i < rows - 1; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				int row = i + (1 - k);
				int index = row*cols + j;
				HeightMapIndices.AddData(&index, sizeof(int));
			}
		}

		//Restart triangle strip
		HeightMapIndices.AddData(&PrimitiveRestartIndex, sizeof(int));
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	HeightMapData.Bind();
	HeightMapData.UploadGPU(GL_STATIC_DRAW);

	//Vertex
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), 0);
	//Texture
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)sizeof(glm::vec3));
	//Normal
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	HeightMapIndices.Bind(GL_ELEMENT_ARRAY_BUFFER);
	HeightMapIndices.UploadGPU(GL_STATIC_DRAW);

	loaded = true;

	return true;
}

void CHeightMap::Release()
{
	if (!loaded)
		return;
	HeightMapData.Release();
	HeightMapIndices.Release();
	glDeleteVertexArrays(1, &vao);
	loaded = false;
}

bool CHeightMap::LoadShaderProgram()
{
	bool ret = true;
	ret = ret & Shader_Vertex.Load("shaders/terrain.vert", GL_VERTEX_SHADER);
	ret = ret & Shader_Fragment.Load("shaders/terrain.frag", GL_FRAGMENT_SHADER);

	Program_Terrain.Create();
	ret = ret & Program_Terrain.AddShader(&Shader_Vertex);
	ret = ret & Program_Terrain.AddShader(&Shader_Fragment);
	ret = ret & Program_Terrain.Link();

	return ret;
}

void CHeightMap::ReleaseShaderProgram()
{
	Program_Terrain.Release();
	Shader_Vertex.Release();
	Shader_Fragment.Release();
}

CShaderProgram* CHeightMap::GetShaderProgram()
{
	return &Program_Terrain;
}

void CHeightMap::SetRenderSize(float x, float h, float z)
{
	RenderScale = glm::vec3(x, h, z);
}

void CHeightMap::SetRenderSize(float QuadSize, float Height)
{
	RenderScale = glm::vec3(float(cols)*QuadSize, Height, float(rows)*QuadSize);
}

int CHeightMap::GetRows()
{
	return rows;
}

int CHeightMap::GetCols()
{
	return cols;
}

void CHeightMap::Render()
{
	Program_Terrain.Use();

	Program_Terrain.SetUniform("fRenderHeight", RenderScale.y);
	Program_Terrain.SetUniform("fMaxTextureU", float(cols)*0.1f);
	Program_Terrain.SetUniform("fMaxTextureV", float(rows)*0.1f);

	Program_Terrain.SetUniform("HeightmapScaleMatrix", glm::scale(glm::mat4(1.0), glm::vec3(RenderScale)));

	glBindVertexArray(vao);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(rows*cols);

	int NumIndices = (rows - 1) * cols * 2 + rows - 1;
	glDrawElements(GL_TRIANGLE_STRIP, NumIndices, GL_UNSIGNED_INT, 0);
}