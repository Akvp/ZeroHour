#include "Skybox.h"

void CSkybox::load(char* front, char* back, char* left, char* right, char* top, char* bottom)
{
	textures[0].load_2D(front);
	textures[1].load_2D(back);
	textures[2].load_2D(left);
	textures[3].load_2D(right);
	textures[4].load_2D(top);
	textures[5].load_2D(bottom);

	this->front = front;
	this->back = back;
	this->left = left;
	this->right = right;
	this->top = top;
	this->bottom = bottom;
	
	for (int i = 0; i<6; i++)
	{
		textures[i].setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR);
		textures[i].setSamplerParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		textures[i].setSamplerParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	vboRenderData.create();
	vboRenderData.bind();

	glm::vec3 SkyBoxVertices[24] =
	{
		// Front face
		glm::vec3(200.0f, 200.0f, 200.0f), glm::vec3(200.0f, -200.0f, 200.0f), glm::vec3(-200.0f, 200.0f, 200.0f), glm::vec3(-200.0f, -200.0f, 200.0f),
		// Back face
		glm::vec3(-200.0f, 200.0f, -200.0f), glm::vec3(-200.0f, -200.0f, -200.0f), glm::vec3(200.0f, 200.0f, -200.0f), glm::vec3(200.0f, -200.0f, -200.0f),
		// Left face
		glm::vec3(-200.0f, 200.0f, 200.0f), glm::vec3(-200.0f, -200.0f, 200.0f), glm::vec3(-200.0f, 200.0f, -200.0f), glm::vec3(-200.0f, -200.0f, -200.0f),
		// Right face
		glm::vec3(200.0f, 200.0f, -200.0f), glm::vec3(200.0f, -200.0f, -200.0f), glm::vec3(200.0f, 200.0f, 200.0f), glm::vec3(200.0f, -200.0f, 200.0f),
		// Top face
		glm::vec3(-200.0f, 200.0f, -200.0f), glm::vec3(200.0f, 200.0f, -200.0f), glm::vec3(-200.0f, 200.0f, 200.0f), glm::vec3(200.0f, 200.0f, 200.0f),
		// Bottom face
		glm::vec3(200.0f, -200.0f, -200.0f), glm::vec3(-200.0f, -200.0f, -200.0f), glm::vec3(200.0f, -200.0f, 200.0f), glm::vec3(-200.0f, -200.0f, 200.0f),
	};
	/*glm::vec2 SkyBoxTexCoords[4] =
	{
		glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)
	};*/
	glm::vec2 SkyBoxTexCoords[4] =
	{
		glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f)
	};

	glm::vec3 SkyBoxNormals[6] =
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
		vboRenderData.addData(&SkyBoxVertices[i], sizeof(glm::vec3));
		vboRenderData.addData(&SkyBoxTexCoords[i % 4], sizeof(glm::vec2));
		vboRenderData.addData(&SkyBoxNormals[i / 4], sizeof(glm::vec3));
	}

	vboRenderData.uploadGPU(GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3) + sizeof(glm::vec2), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

void CSkybox::render()
{
	glDepthMask(0);
	glBindVertexArray(VAO);
	for (int i = 0; i < 6; i++)
	{
		textures[i].bind();
		glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
	}
	glDepthMask(1);
}

void CSkybox::release()
{
	for (int i = 0; i < 6; i++)
	{
		textures[i].release();
	}
	vboRenderData.release();
}