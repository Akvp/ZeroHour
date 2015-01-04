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

	void Create(int size = 0);
	void Release();

	void* MapBuffer(int hint);
	void* MapSubBuffer(int hint, GLuint offset, GLuint length);
	void UnmapBuffer();

	void Bind(int type = GL_ARRAY_BUFFER);
	void UploadGPU(int hint);

	void AddData(void* ptrData, GLuint size);

	void* GetDataPointer();
	GLuint GetID();

	int GetCurrentSize();

private:
	GLuint buffer;
	int size;
	int currentSize;
	int type;
	std::vector<BYTE> data;

	bool dataUploaded;
};

#endif