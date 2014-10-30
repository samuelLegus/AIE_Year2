#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include <stb_image.h>

// derived application class that wraps up all globals neatly
class Tutorial4 : public Application
{
public:

	Tutorial4();
	virtual ~Tutorial4();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;
	unsigned int m_renderProgram;
	unsigned int m_texture;
	unsigned int m_vertShader;
	unsigned int m_fragShader;
};