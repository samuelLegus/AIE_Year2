#pragma once

#include "Behaviour.h"

class Agent
{
public:
	Agent() {}
	virtual ~Agent() {}

private:
	Behaviour * m_behaviour;
	glm::vec3 m_position;
};