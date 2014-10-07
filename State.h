#ifndef _STATE_H_
#define _STATE_H_

#include "Object.h"

class State
{
public:
	State();
	virtual ~State();

	virtual void Enter(Object*) = 0;
	virtual void Execute(Object*) = 0;
	virtual void Exit(Object*) = 0;
};

class RoamState : public State
{
public:
	RoamState();
	void Enter(Object* agent);
	void Execute(Object* agent);
	void Exit(Object* agent);
};

class FleeState : public State
{

};

#endif