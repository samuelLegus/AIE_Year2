#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include <FBXFile.h>

struct OGL_FBXRenderData
{
	unsigned int VBO;
	unsigned int IBO;
	unsigned int VAO;
};

// derived application class that wraps up all globals neatly
class Tutorial3 : public Application
{
public:

	Tutorial3();
	virtual ~Tutorial3();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	//Stuff from the FBX Tutorial
	unsigned int m_ShaderProgram;

	float m_decayValue;
	unsigned int m_decayTexture;

	void InitFBXSceneResource(FBXFile * a_pScene);
	void UpdateFBXSceneResource(FBXFile * a_pScene);
	void RenderFBXSceneResource(FBXFile * a_pScene, glm::mat4 a_view, glm::mat4 a_projection);
	void DestroyFBXSceneResource(FBXFile * a_pScene);

	FBXFile * m_FBX;



};