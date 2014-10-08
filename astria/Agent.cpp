#include "State.h"
#include "utils.h"

Agent::Agent()
{
	Agent("Individual " + std::to_string(NextValidID));
}

Agent::Agent(std::string name)
{
	int x = RandInt(CParams::WorldMinX, CParams::WorldMaxX);
	int y = RandInt(CParams::WorldMinY, CParams::WorldMaxY);
	int z = CParams::GroundLevel;
	Agent(name, x, y, z);
}

Agent::Agent(std::string name, int x, int y, int z)
{
	Name = name;
	Position = glm::vec3(x, y, z);
	Look = glm::vec3(0) - Position;
	MoveDirection = glm::vec3(0);
	Speed = 0;

	CurrentState = new State_Roam();
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