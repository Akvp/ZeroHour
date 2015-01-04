#include "VBO.h"

CVBO::CVBO()
{
	dataUploaded = false;
	buffer = 0;
}

void CVBO::Create(int size)
{
	glGenBuffers(1, &buffer);
	data.reserve(size);
	this->size = size;
	currentSize = 0;
}

void CVBO::Release()
{
	glDeleteBuffers(1, &buffer);
	dataUploaded = false;
	data.clear();
}

void* CVBO::MapBuffer(int hint)
{
	if (!dataUploaded)
		return NULL;
	void* ptrRes = glMapBuffer(type, hint);
	return ptrRes;
}

void* CVBO::MapSubBuffer(int hint, GLuint offset, GLuint length)
{
	if (!dataUploaded)
		return NULL;
	void* ptrRes = glMapBufferRange(type, offset, length, hint);
	return ptrRes;
}

void CVBO::UnmapBuffer()
{
	glUnmapBuffer(type);
}

void CVBO::Bind(int type)
{
	this->type = type;
	glBindBuffer(type, buffer);
}

void CVBO::UploadGPU(int hint)
{
	glBufferData(type, data.size(), &data[0], hint);
	dataUploaded = true;
	data.clear();
}

void CVBO::AddData(void* ptrData, GLuint size)
{
	data.insert(data.end(), (BYTE*)ptrData, (BYTE*)ptrData + size);
	currentSize += size;
}

void* CVBO::GetDataPointer()
{
	if (dataUploaded)
		return NULL;
	return (void*)data[0];
}

GLuint CVBO::GetID()
{
	return buffer;
}

int CVBO::GetCurrentSize()
{
	return currentSize;
}