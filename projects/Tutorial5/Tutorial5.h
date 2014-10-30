#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include "ParticleSystem.h"
#include "Shader.h"

// derived application class that wraps up all globals neatly
class Tutorial5 : public Application
{
public:

	Tutorial5();
	virtual ~Tutorial5();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	unsigned int m_shader;

	ParticleEmitter * m_emitter;
};