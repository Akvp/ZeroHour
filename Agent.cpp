#include "State.h"

#include "Agent.h"

Agent::Agent()
{
	string name = "Individual " + NextValidID;
	Agent(name);
}

Agent::Agent(string name)
{
	int x = RandInt(0, CParams::WorldX);
	int y = RandInt(0, CParams::WorldY);
	int z = CParams::GroundLevel;
	Agent(name, x, y, z);
}

Agent(string name, int x, int y, int z)
{
	Name = name;
	Position = vec3(x, y, z);
	Look = vec3(random);
	MoveDirection = vec3(0);
	Speed = 0;

	CurrentState = new State()
}

Agent::~Agent()
{

}

void Agent::ChangeState(State* newState)
{
	assert(CurrentState && newState);

	CurrentState->Exit(this);
	CurrentState = newState;
	CurrentState->Enter(this);
}