#pragma once

#include "Application.h"
#include "PhysicsEngine.h"
#include <glm/glm.hpp>
#include "Actors.h"


// derived application class that wraps up all globals neatly
class PhysicsCustom : public Application
{
public:

	PhysicsCustom();
	virtual ~PhysicsCustom();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	PhysicsEngine * m_engine;

};

