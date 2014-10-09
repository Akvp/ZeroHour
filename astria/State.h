#ifndef _STATE_H_
#define _STATE_H_

class Agent;

class State
{
public:
	State();
	virtual ~State();

	virtual void Enter(Agent*) = 0;
	virtual void Execute(Agent*) = 0;
	virtual void Exit(Agent*) = 0;
};

class State_Roam : public State
{
public:
	State_Roam();
	void Enter(Agent* agent);
	void Execute(Agent* agent);
	void Exit(Agent* agent);
};

class State_Flee : public State
{
public:
	State_Flee();
	void Enter(Agent* agent);
	void Execute(Agent* agent);
	void Exit(Agent* agent);
};

#endif