#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include <FBXFile.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// derived application class that wraps up all globals neatly

//\============================================================================
//\ A Simple Vertex Structure
//\  - Two integers for position
//\  - Four floats for colour (colour is required to be a float array of 16 bytes 
//\    length in all recent versions of openGL)
//\============================================================================
struct VertexBasic
{
	glm::vec4 position;
	glm::vec4 colour;
};


class Tutorial1 : public Application
{
public:

	Tutorial1();
	virtual ~Tutorial1();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	GLuint m_VBO;
	GLuint m_IBO;
	GLuint m_VAO;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};