#ifndef _AGENT_H_
#define _AGENT_H_

#include "Object.h"
#include "Common.h"

enum AGENT_STATE
{
	STATE_ROAM,
	STATE_FLEE
};

class State;

class Agent : public Object
{
public:
	Agent();
	Agent(std::string name);
	Agent(std::string name, int x, int y, int z);
	~Agent();

	void Move(float direction, float distance);

	Object* FindTarget();
	void CenterTarget();

	void ChangeState(State* newState);
	void Interact(Object* target);

	void Update();

	std::vector<Object*> GetSurrounding(float radius);
	std::vector<Object*> GetSurrounding(int numObj);

// Accessor functions
public:
	glm::vec3 GetMoveDirection () const { return MoveDirection; }
	glm::vec3 GetLook () const { return Look; }
	float GetSpeed () const { return Speed; }
	float GetTurnRate() const { return TurnRate; }
	State* GetState () const { return CurrentState; }

private:
	std::string Name;

	glm::vec3 Look;
	glm::vec3 MoveDirection;
	float Speed;
	float TurnRate;

	State* CurrentState;
};

#endif