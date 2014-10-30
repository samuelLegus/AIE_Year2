#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include <FBXFile.h>
#include <GL/glew.h>
#include <glm/ext.hpp>
#include "Utilities.h"
#include "Texture.h"

struct OGL_FBXRenderData
{
	unsigned int VBO;
	unsigned int IBO;
	unsigned int VAO;
};

// derived application class that wraps up all globals neatly
class Assessment : public Application
{
public:

	Assessment();
	virtual ~Assessment();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

};




