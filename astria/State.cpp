#include "State.h"
#include "Agent.h"

State::State()
{

}

State::~State()
{

}

// ===== Pure virtual functions =====

void State::Enter(Agent*)
{

}

void State::Execute(Agent*)
{

}

void State::Exit(Agent*)
{

}

// ===== Roam state =====

State_Roam::State_Roam()
{

}

void State_Roam::Enter(Agent* agent)
{

}

void State_Roam::Execute(Agent* agent)
{

}

void State_Roam::Exit(Agent* agent)
{

}