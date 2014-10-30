#pragma once

#include "Application.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "MyUtils.h"

struct VertexBasic
{
	glm::vec4 position;
	glm::vec4 colour;
};

class Tutorial2 : public Application
{
public:

	Tutorial2();
	virtual ~Tutorial2();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	GLuint m_VBO;
	GLuint m_IBO;
	GLuint m_VAO;
	GLuint m_ShaderProgram;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	void GenerateGrid(unsigned int rows, unsigned int cols);
};