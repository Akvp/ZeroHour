#ifndef _VBO_H_
#define _VBO_H_

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <Windows.h>

class CVBO
{
public:
	CVBO();

	void create(int size = 0);
	void release();

	void* mapBuffer(int hint);
	void* mapSubBuffer(int hint, GLuint offset, GLuint length);
	void unmapBuffer();

	void bind(int type = GL_ARRAY_BUFFER);
	void uploadGPU(int hint);

	void addData(void* ptrData, GLuint size);

	void* getDataPointer();
	GLuint getID();

	int getCurrentSize();

private:
	GLuint buffer;
	int size;
	int currentSize;
	int type;
	std::vector<BYTE> data;

	bool dataUploaded;
};

#endif