#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "Common.h"

enum OBJ_FLAG
{
	FLAG_NONE		= 0x00000000,
	FLAG_GRAVITY	= 0x00000002,
	FLAG_GHOST		= 0x00000004
};

typedef unsigned int uint;

class Object
{
public:
	Object() {}
	virtual ~Object() {}
	glm::vec3 GetPosition () const { return this->Position; }
	OBJ_FLAG GetFlag () const { return Flag; }
	uint GetID () const { return ID; }

	virtual void Update() = 0;

protected:
	glm::vec3 Position;
	OBJ_FLAG Flag;
	uint ID;
	static uint NextValidID;
};

#endif