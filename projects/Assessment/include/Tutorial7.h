#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include "GPUParticleSystem.h"

// derived application class that wraps up all globals neatly
class Tutorial7 : public Application
{
public:

	Tutorial7();
	virtual ~Tutorial7();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	GPUParticleEmitter * m_emitter;


	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

