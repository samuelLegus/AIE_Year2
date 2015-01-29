#pragma once
#include <vector>

class Agent; //forward declare

class Behaviour
{
public:
	Behaviour() {}
	virtual ~Behaviour() {}

	virtual bool execute(Agent * agent) = 0; //pure abstract class
};

class Composite : public Behaviour
{
public:
	Composite() {}
	virtual ~Composite() {}

	void addChild(Behaviour* behaviour)
	{
		m_children.push_back(behaviour);
	}

protected:

	std::vector<Behaviour*> m_children;

};

class Selector : public Composite
{
public:
	Selector() {}
	virtual ~Selector() {}

	//loop thru all children and return if any child's execute function succeeds
	virtual bool exectute(Agent * agent)
	{
		for (auto behaviour : m_children)
		{
			if (behaviour->execute(agent) == true)
			{
				return true;
			}
		}
		return false;
	}
}