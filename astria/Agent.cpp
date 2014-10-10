#include "State.h"
#include "Agent.h"
#include "utils.h"

Agent::Agent()
{
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

void Agent::Update()
{

}

void Agent::ChangeState(State* newState)
{
	assert(CurrentState && newState);

	CurrentState->Exit(this);
	CurrentState = newState;
	CurrentState->Enter(this);
}