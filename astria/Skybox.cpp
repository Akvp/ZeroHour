#include "Common.h"

#include "Skybox.h"

/*-----------------------------------------------

Name:	LoadSkybox

Params:	paths of skybox pictures

Result:	Loads skybox and creates VAO and VBO for it.

/*---------------------------------------------*/

void CSkybox::load(std::string a_sDirectory, std::string a_sFront, std::string a_sBack, std::string a_sLeft, std::string a_sRight, std::string a_sTop, std::string a_sBottom)
{
	tTextures[0].load_2D(a_sDirectory + a_sFront);
	tTextures[1].load_2D(a_sDirectory + a_sBack);
	tTextures[2].load_2D(a_sDirectory + a_sLeft);
	tTextures[3].load_2D(a_sDirectory + a_sRight);
	tTextures[4].load_2D(a_sDirectory + a_sTop);
	tTextures[5].load_2D(a_sDirectory + a_sBottom);

	sDirectory = a_sDirectory;

	sFront = a_sFront;
	sBack = a_sBack;
	sLeft = a_sLeft;
	sRight = a_sRight;
	sTop = a_sTop;
	sBottom = a_sBottom;

	for (int i = 0; i < 6; i++)
	{
		tTextures[i].setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR);
		tTextures[i].setSamplerParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		tTextures[i].setSamplerParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glGenVertexArrays(1, &uiVAO);
	glBindVertexArray(uiVAO);

	vboRenderData.create();
	vboRenderData.bind();

	//All vertices
	//8 vertices of a cube, repeated 3 times each
	glm::vec3 vSkyBoxVertices[24] = 
	{
		// Front face
		glm::vec3(50.0f, 50.0f, 50.0f), glm::vec3(50.0f, -50.0f, 50.0f), glm::vec3(-50.0f, 50.0f, 50.0f), glm::vec3(-50.0f, -50.0f, 50.0f),
		// Back face
		glm::vec3(-50.0f, 50.0f, -50.0f), glm::vec3(-50.0f, -50.0f, -50.0f), glm::vec3(50.0f, 50.0f, -50.0f), glm::vec3(50.0f, -50.0f, -50.0f),
		// Left face
		glm::vec3(-50.0f, 50.0f, 50.0f), glm::vec3(-50.0f, -50.0f, 50.0f), glm::vec3(-50.0f, 50.0f, -50.0f), glm::vec3(-50.0f, -50.0f, -50.0f),
		// Right face
		glm::vec3(50.0f, 50.0f, -50.0f), glm::vec3(50.0f, -50.0f, -50.0f), glm::vec3(50.0f, 50.0f, 50.0f), glm::vec3(50.0f, -50.0f, 50.0f),
		// Top face
		glm::vec3(-50.0f, 50.0f, -50.0f), glm::vec3(50.0f, 50.0f, -50.0f), glm::vec3(-50.0f, 50.0f, 50.0f), glm::vec3(50.0f, 50.0f, 50.0f),
		// Bottom face
		glm::vec3(50.0f, -50.0f, -50.0f), glm::vec3(-50.0f, -50.0f, -50.0f), glm::vec3(50.0f, -50.0f, 50.0f), glm::vec3(-50.0f, -50.0f, 50.0f),
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
		vboRenderData.addData(&vSkyBoxVertices[i], sizeof(glm::vec3));
		vboRenderData.addData(&vSkyBoxTexCoords[i%4], sizeof(glm::vec2));
		vboRenderData.addData(&vSkyBoxNormals[i/4], sizeof(glm::vec3));
	}

	vboRenderData.uploadGPU(GL_STATIC_DRAW);

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

//void CSkybox::load(std::string a_sDirectory, std::string a_sFront, std::string a_sBack, std::string a_sLeft, std::string a_sRight, std::string a_sTop, std::string a_sBottom)
//{
//	sDirectory = a_sDirectory;
//
//	sFront = a_sFront;
//	sBack = a_sBack;
//	sLeft = a_sLeft;
//	sRight = a_sRight;
//	sTop = a_sTop;
//	sBottom = a_sBottom;
//
//	glActiveTexture(GL_TEXTURE0);
//	glGenTextures(1, &tex_cube);
//
//
//
//	load_texture(sDirectory + sFront, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
//	load_texture(sDirectory + sBack, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
//	load_texture(sDirectory + sTop, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
//	load_texture(sDirectory + sBottom, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
//	load_texture(sDirectory + sLeft, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
//	load_texture(sDirectory + sRight, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
//
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//
//	float points[] = {
//		-10.0f, 10.0f, -10.0f,
//		-10.0f, -10.0f, -10.0f,
//		10.0f, -10.0f, -10.0f,
//		10.0f, -10.0f, -10.0f,
//		10.0f, 10.0f, -10.0f,
//		-10.0f, 10.0f, -10.0f,
//
//		-10.0f, -10.0f, 10.0f,
//		-10.0f, -10.0f, -10.0f,
//		-10.0f, 10.0f, -10.0f,
//		-10.0f, 10.0f, -10.0f,
//		-10.0f, 10.0f, 10.0f,
//		-10.0f, -10.0f, 10.0f,
//
//		10.0f, -10.0f, -10.0f,
//		10.0f, -10.0f, 10.0f,
//		10.0f, 10.0f, 10.0f,
//		10.0f, 10.0f, 10.0f,
//		10.0f, 10.0f, -10.0f,
//		10.0f, -10.0f, -10.0f,
//
//		-10.0f, -10.0f, 10.0f,
//		-10.0f, 10.0f, 10.0f,
//		10.0f, 10.0f, 10.0f,
//		10.0f, 10.0f, 10.0f,
//		10.0f, -10.0f, 10.0f,
//		-10.0f, -10.0f, 10.0f,
//
//		-10.0f, 10.0f, -10.0f,
//		10.0f, 10.0f, -10.0f,
//		10.0f, 10.0f, 10.0f,
//		10.0f, 10.0f, 10.0f,
//		-10.0f, 10.0f, 10.0f,
//		-10.0f, 10.0f, -10.0f,
//
//		-10.0f, -10.0f, -10.0f,
//		-10.0f, -10.0f, 10.0f,
//		10.0f, -10.0f, -10.0f,
//		10.0f, -10.0f, -10.0f,
//		-10.0f, -10.0f, 10.0f,
//		10.0f, -10.0f, 10.0f
//	};
//	vboRenderData.create();
//	vboRenderData.bind();
//	vboRenderData.addData(points, 3 * 36 * sizeof(float));
//	vboRenderData.uploadGPU(GL_STATIC_DRAW);
//
//	glGenVertexArrays(1, &uiVAO);
//	glBindVertexArray(uiVAO);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
//}

void CSkybox::load_texture(std::string file, GLenum side)
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex_cube);

	int w, h;
	
	SDL_Surface* Surf_Load = IMG_Load(file.c_str());
	if (Surf_Load == NULL)
	{
		char errorMsg[512] = "";
		sprintf(errorMsg, "Error loading %s!\nError message: %s\n", file, IMG_GetError());
		MessageBox(NULL, errorMsg, "Texture loading error", MB_ICONERROR);
		return;
	}

	w = Surf_Load->w;
	h = Surf_Load->h;
	
	// non-power-of-2 dimensions check
	if ((w & (w - 1)) != 0 || (h & (h - 1)) != 0) {
		fprintf(stderr, "WARNING: image %s is not power-of-2 dimensions\n", file.c_str());
	}

	glTexImage2D(
		side,
		0,
		GL_RGB,
		w,
		h,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		Surf_Load->pixels);
}

/*-----------------------------------------------

Name:	RenderSkybox

Params:	none

Result: Guess what it does :)

/*---------------------------------------------*/

void CSkybox::render()
{
	glDepthMask(0);
	glBindVertexArray(uiVAO);
	for (int i = 0; i < 6; i++)
	{
		tTextures[i].bind();
		glDrawArrays(GL_TRIANGLE_STRIP, i*4, 4);
	}
	glDepthMask(1);

	//glDepthMask(GL_FALSE);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, tex_cube);
	//glBindVertexArray(uiVAO);
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	//glDepthMask(GL_TRUE);
}

/*-----------------------------------------------

Name:	DeleteSkybox

Params:	none

Result:	Deletes skybox and all associated memory.

/*---------------------------------------------*/

void CSkybox::release()
{
	for(int i = 0; i < 6; i++)tTextures[i].release();
	glDeleteVertexArrays(1, &uiVAO);
	vboRenderData.release();
}