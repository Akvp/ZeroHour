#include "StaticObj.h"

glm::vec2 vCubeTexCoords[6] = { glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f) };

glm::vec3 vGround[6] =
{
	glm::vec3(-1000, 0, -1000), glm::vec3(-1000, 0, 1000), glm::vec3(1000, 0, 1000), glm::vec3(1000, 0, 1000), glm::vec3(1000, 0, -1000), glm::vec3(-1000, 0, -1000)
};

void CreateStaticSceneObjects(GLuint* VAO, CVBO& vboDest)
{
	vboDest.create();
	glGenVertexArrays(1, VAO);
	glBindVertexArray(*VAO);

	vboDest.bind();

	//Add ground to VBO
	for (int i = 0; i < 6; i++)
	{
		vboDest.addData(&vGround[i], sizeof(glm::vec3));
		glm::vec2 vCoord = vCubeTexCoords[i] * 50.0f;
		vboDest.addData(&vCoord, sizeof(glm::vec2));
		glm::vec3 vGroundNormal(0.0f, 1.0f, 0.0f);
		vboDest.addData(&vGroundNormal, sizeof(glm::vec3));
	}

	vboDest.uploadGPU(GL_STATIC_DRAW);

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