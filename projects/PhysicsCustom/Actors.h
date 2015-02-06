#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#include "Gizmos.h"
#include "Utilities.h"

enum eVolumeType
{
	PLANE,
	BALL,
	BOX
};

class Actor
{
public:

	glm::vec3 m_position;
	glm::vec3 m_velocity;
	glm::quat m_rotation;
	glm::vec4 m_colour;

	float m_width;
	float m_height;
	bool m_static;
	int m_collisionID;
	float m_mass;

	eVolumeType m_eVolumeType;

	Actor(glm::vec3 pos, float mass, int collisionID ,glm::quat rot = glm::quat(0,0,0,0) , glm::vec3 initForce = glm::vec3(0, 0, 0), glm::vec4 colour = glm::vec4(1, 1, 1, 1), bool isStatic = false)
	{
		m_position = pos;
		m_velocity = glm::vec3(0, 0, 0);
		m_rotation = rot;
		m_mass = mass;
		m_colour = colour;
		m_static = isStatic;
		m_collisionID = collisionID;

		applyForce(initForce);
	}

	void onUpdate(glm::vec3 gravity, float deltaTime)
	{
		if (!m_static)
		{
			applyForce(gravity * m_mass * deltaTime);
			m_position += m_velocity * deltaTime;
		}
	}

	void applyForce(glm::vec3 force)
	{
		m_velocity += force / m_mass;
	}

	//This is strange to me -_-
	void applyForceToActor(Actor * other, glm::vec3 force)
	{
		other->applyForce(force);
		applyForce(-force);
	}

	virtual void onDraw() = 0;

};

class Plane : public Actor
{
public:

	float m_width;
	float m_length;

	//By default planes are not gonna move. 
	Plane(glm::vec3 pos, float w, float l, float mass, int collisionID,glm::vec3 initForce = glm::vec3(0, 0, 0), glm::vec4 colour = glm::vec4(1, 0, 1, .75f), bool isStatic = true)
		: Actor(pos, mass, collisionID, glm::quat(0,0,0,0), initForce, colour, isStatic)
	{
		m_width = w;
		m_length = l;
		m_eVolumeType = PLANE;
	}

	void onDraw()
	{
		Gizmos::addAABBFilled(m_position, glm::vec3(m_width, .1, m_length), m_colour);
	}

};

class Ball : public Actor
{
protected:

public:

	float m_radius;

	Ball(glm::vec3 pos, float radius, float mass, int collisionID, glm::vec3 initForce = glm::vec3(0, 0, 0), glm::vec4 colour = glm::vec4(0, 1, 1, .75f), bool isStatic = false)
	: Actor(pos, mass, collisionID, glm::quat(0, 0, 0, 0), initForce, colour, isStatic)
	{
		m_radius = radius;
		m_eVolumeType = BALL;
	}

	void onDraw()
	{
		Gizmos::addSphere(m_position, m_radius, 10, 10, m_colour);
	}

};

class Box : public Actor
{
protected:

public:

	float m_width;
	float m_height;
	float m_length;

	Box(glm::vec3 pos, float w, float h, float l, float mass, int collisionID, glm::vec3 initForce = glm::vec3(0, 0, 0), glm::vec4 colour = glm::vec4(1, 1, 0, .75f), bool isStatic = false)
		: Actor(pos, mass, collisionID, glm::quat(0, 0, 0, 0), initForce, colour, isStatic)
	{
		m_width = w;
		m_height = h;
		m_length = l;
		m_eVolumeType = BOX;
	}

	void onDraw()
	{
		Gizmos::addAABBFilled(m_position, glm::vec3(m_width, m_height, m_length), m_colour);
	}

};
