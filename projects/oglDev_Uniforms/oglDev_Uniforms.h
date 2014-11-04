#pragma once

#include "Application.h"
#include <glm/glm.hpp>

// derived application class that wraps up all globals neatly
class oglDev_Uniforms : public Application
{
public:

	oglDev_Uniforms();
	virtual ~oglDev_Uniforms();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	//tutorial stuff

	unsigned int m_vbo;
	unsigned int m_program;
	unsigned int m_gScaleLocation;

	void CreateVertexBuffer();
	void CreateShaderProgram();
	void RenderTriangle();
};