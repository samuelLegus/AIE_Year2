#pragma once

#include <glm/glm.hpp>
#include "Actors.h"

class PhysicsEngine
{
private:

	int m_collisionIDs;
	glm::vec3 m_gravity; // So that can we can sideways gravity and stuff, default gravity will be downward.


public:

	std::vector<Actor*> m_actors;

	PhysicsEngine(glm::vec3 gravity)
	{
		m_collisionIDs = 0;
		m_gravity = gravity;

		
	}

	void debugVolumes()
	{
		system("cls");
		printf("ACTIVE VOLUMES :\n");
		for (auto actor : m_actors)
		{
			switch (actor->m_eVolumeType)
			{
			case BALL:
				printf("	BALL, ID = %i\n", actor->m_collisionID);
				break;
			case PLANE:
				printf("	PLANE, ID = %i\n", actor->m_collisionID);
				break;
			case BOX:
				printf("	BOX, ID = %i\n", actor->m_collisionID);
				break;
			}
		}
	}

	void addBall(glm::vec3 position, float radius, float mass, glm::vec3 initForce = glm::vec3(0, 0, 0))
	{
		m_actors.push_back(new Ball(position, radius, mass, m_collisionIDs++, initForce));
	}

	void addPlane(glm::vec3 position, float w, float l, float mass)
	{
		m_actors.push_back(new Plane(position, w, l, mass, m_collisionIDs++));
	}

	void addBox(glm::vec3 position, float w, float l, float h, float mass, glm::vec3 initForce = glm::vec3(0,0,0))
	{
		m_actors.push_back(new Box(position, w, l, h, mass, m_collisionIDs++, initForce));
	}

	void onDraw()
	{
		for (auto actor : m_actors)
		{
			actor->onDraw();
		}
	}

	void onUpdate(float deltaTime)
	{
		for (auto actor : m_actors)
		{
			actor->onUpdate(m_gravity, deltaTime);
		}
	}

	void onDestroy();

	//TO:DO Handle this later...
	void handleCollision(Actor * a, Actor * b)
	{
		switch (a->m_eVolumeType)
		{
		case BALL:
		{
					 Ball * first = dynamic_cast<Ball *>(a);
					 switch (b->m_eVolumeType)
					 {
					 case BALL:
					 {
								  Ball * second = dynamic_cast<Ball *>(b);
								  float sumRadius = first->m_radius + second->m_radius;
								  float distance = glm::distance(first->m_position, second->m_position);
								  if (distance <= sumRadius)
								  {
									  printf("SPHERE + SPHERE Collision!\n");
									  //
								  }
					 }
					 }
		}
		}
	}

};