#ifndef _AGENT_H_
#define _AGENT_H_

#include "SVector2D.h"
#include "Object.h"

enum AGENT_STATE
{
	STATE_ROAM,
	STATE_FLEE
};

class Agent : public Object
{
public:
	Agent();
	Agent(string name);
	Agent(string name, int x, int y, int z);
	~Agent();

	void Move(float direction, float distance);

	void ChangeState(State* newState);
	void Interact(Object* target);

	void Update();

	vector<Object*> GetSurrounding(float radius);
	vector<Object*> GetSurrounding(int numObj);

// Accessor functions
public:
	vec3 GetPosition const () { return Position; }
	vec3 GetMoveDirection const () { return MoveDirection; }
	vec3 GetLook const () { return Look; }
	float GetSpeed const () { return Speed; }
	State* GetState const () { return CurrentState; }

private:
	string Name;

	vec3 Look;
	vec3 MoveDirection;
	float Speed;

	State* CurrentState;


};

#endif