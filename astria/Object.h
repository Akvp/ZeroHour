#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "Common.h"

class Object
{
public:
	virtual ~Object(){}
protected:
	glm::vec3 Position;
};

#endif