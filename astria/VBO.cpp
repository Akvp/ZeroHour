#include "VBO.h"

CVBO::CVBO()
{
	dataUploaded = false;
	buffer = 0;
}

void CVBO::create(int size)
{
	glGenBuffers(1, &buffer);
	data.reserve(size);
	this->size = size;
	currentSize = 0;
}

void CVBO::release()
{
	glDeleteBuffers(1, &buffer);
	dataUploaded = false;
	data.clear();
}

void* CVBO::mapBuffer(int hint)
{
	if (!dataUploaded)
		return NULL;
	void* ptrRes = glMapBuffer(type, hint);
	return ptrRes;
}

void* CVBO::mapSubBuffer(int hint, GLuint offset, GLuint length)
{
	if (!dataUploaded)
		return NULL;
	void* ptrRes = glMapBufferRange(type, offset, length, hint);
	return ptrRes;
}

void CVBO::unmapBuffer()
{
	glUnmapBuffer(type);
}

void CVBO::bind(int type)
{
	this->type = type;
	glBindBuffer(type, buffer);
}

void CVBO::uploadGPU(int hint)
{
	glBufferData(type, data.size(), &data[0], hint);
	dataUploaded = true;
	data.clear();
}

void CVBO::addData(void* ptrData, GLuint size)
{
	data.insert(data.end(), (BYTE*)ptrData, (BYTE*)ptrData + size);
	currentSize += size;
}

void* CVBO::getDataPointer()
{
	if (dataUploaded)
		return NULL;
	return (void*)data[0];
}

GLuint CVBO::getID()
{
	return buffer;
}

int CVBO::getCurrentSize()
{
	return currentSize;
}