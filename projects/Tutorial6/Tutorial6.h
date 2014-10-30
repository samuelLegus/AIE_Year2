#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include <FBXFile.h>
#include "Shader.h"

struct DirectionalLight
{
	glm::vec4 m_direction = glm::vec4(0, 1, 0, 0);
	glm::vec4 m_color = glm::vec4(1, 0, 0, 1);
};

struct PointLight
{
	glm::vec4 m_origin = glm::vec4(0, 0, 0, 0);
	glm::vec4 m_direction = glm::vec4(0, 0, 0, 0); 
	glm::vec4 m_color = glm::vec4(0, 0, 0, 0);
};

// derived application class that wraps up all globals neatly
class Tutorial6 : public Application
{
public:

	Tutorial6();
	virtual ~Tutorial6();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;


	FBXFile * m_fbx;
	unsigned int m_shader;
	DirectionalLight m_dirLight;
	glm::vec3 m_ambLight;
	glm::vec3 m_specLight;
};

