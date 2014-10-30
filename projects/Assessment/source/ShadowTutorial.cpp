#include "ShadowTutorial.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

ShadowTutorial::ShadowTutorial()
{
}

ShadowTutorial::~ShadowTutorial()
{

}

void ShadowTutorial::BuildProgram()
{
	unsigned int vs = Utility::loadShader("shaders/ShadowMapRenderVS.glsl", GL_VERTEX_SHADER);
	unsigned int fs = Utility::loadShader("shaders/ShadowMapRenderFS.glsl", GL_FRAGMENT_SHADER);
	m_shader = Utility::createProgram(vs, 0, 0, 0, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);
}

void ShadowTutorial::BuildDepthProgram()
{
	unsigned int vs = Utility::loadShader("shaders/ShadowMapRTT_VS.glsl", GL_VERTEX_SHADER);
	unsigned int fs = Utility::loadShader("shaders/ShadowMapRTT_FS.glsl", GL_FRAGMENT_SHADER);
	m_depthShader = Utility::createProgram(vs, 0, 0, 0, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);
}

void ShadowTutorial::CreateShadowBuffer()
{
	// resolution of the texture to put our shadow map in
	m_shadowWidth = 1024;
	m_shadowHeight = 1024;

	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	glGenFramebuffers(1, &m_shadowFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFramebuffer);

	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	glGenTextures(1, &m_uShadowMap);
	glBindTexture(GL_TEXTURE_2D, m_uShadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, m_shadowWidth, m_shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_uShadowMap, 0);

	glDrawBuffer(GL_NONE); // No color buffer is drawn to.

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("depth buffer not created");
	}
	else
	{
		printf("Success! created depth buffer\n");
	}

	// return to back-buffer rendering
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowTutorial::CalculateLightAndShadowMatrix()
{
		// setup light direction and shadow matrix
	glm::vec3 lightPosition = glm::vec3(1.0f,1.0f,0);
	m_lightDirection = glm::normalize(glm::vec4( -lightPosition, 0 ));

	glm::mat4 depthViewMatrix = glm::lookAt(lightPosition, glm::vec3(0,0,0), glm::vec3(0,1,0));
	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-3,3,-3,3,-50,50);
	m_depthMVP = depthProjectionMatrix * depthViewMatrix;
}


bool ShadowTutorial::onCreate(int a_argc, char * a_argv[])
{
	Assessment::onCreate(a_argc, a_argv);

	//
	BuildDepthProgram();
	m_uDepthMVP = glGetUniformLocation(m_depthShader, "depthMVP");
	//
	m_fbx = new FBXFile();
	if (!m_fbx->load("models/soulspear/soulspear.fbx", FBXFile::UNITS_CENTIMETER))
	{
		printf("FBX file could not be loaded!");
	}
	m_fbx->initialiseOpenGLTextures();
	InitFBXSceneResource(m_fbx);
	CreateShadowBuffer();

	BuildProgram();

	m_uTexture = glGetUniformLocation(m_shader, "myTextureSampler");
	m_uMVP = glGetUniformLocation(m_shader, "MVP");
	m_uDepthBiasMVP = glGetUniformLocation(m_shader, "DepthBiasMVP");
	m_uShadowMap = glGetUniformLocation(m_shader, "shadowMap");

	return true;
}

void ShadowTutorial::onUpdate(float a_deltaTime)
{
	Assessment::onUpdate(a_deltaTime);
	UpdateFBXSceneResource(m_fbx);
}

void ShadowTutorial::onDraw()
{
	Assessment::onDraw();

	//Rendering to framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFramebuffer);
	glUseProgram(m_depthShader);
	CalculateLightAndShadowMatrix();
	glUniformMatrix4fv(m_uDepthMVP, 1, false, glm::value_ptr(m_depthMVP));

	//stupid framework layout making me do this -_-

	unsigned int meshCount = m_fbx->getMeshCount();
	unsigned int matCount = m_fbx->getMaterialCount();

	// loop over meshes
	for (int i = 0; i < meshCount; ++i)
	{
		//render to the framebuffer

		// get current mesh
		FBXMeshNode * pMesh = m_fbx->getMeshByIndex(i);

		OGL_FBXRenderData * ro = new OGL_FBXRenderData();
		pMesh->m_userData = ro;

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, ro->VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char*)FBXVertex::PositionOffset);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ro->IBO);
		glDisableVertexAttribArray(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	RenderFBXSceneResource(m_fbx, glm::inverse(m_cameraMatrix) , m_projectionMatrix);

}

void ShadowTutorial::onDestroy()
{
	Assessment::onDestroy();
}

void ShadowTutorial::InitFBXSceneResource(FBXFile * a_pScene)
{
	unsigned int meshCount = a_pScene->getMeshCount();
	unsigned int matCount = a_pScene->getMaterialCount();

	// loop over meshes
	for (int i = 0; i < meshCount; ++i)
	{
		//render to the framebuffer

		// get current mesh
		FBXMeshNode * pMesh = a_pScene->getMeshByIndex(i);

		OGL_FBXRenderData * ro = new OGL_FBXRenderData();
		pMesh->m_userData = ro;

		// generate buffers
		glGenBuffers(1, &ro->VBO);
		glGenBuffers(1, &ro->IBO);
		glGenVertexArrays(1, &ro->VAO);

		// bind VAO, then VBO and IBO to VAO
		glBindVertexArray(ro->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, ro->VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ro->IBO);

		// send data to VBO
		glBufferData(GL_ARRAY_BUFFER, pMesh->m_vertices.size() * sizeof(FBXVertex), pMesh->m_vertices.data(), GL_STATIC_DRAW);

		// send data to IBO
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, pMesh->m_indices.size() * sizeof(unsigned int), pMesh->m_indices.data(), GL_STATIC_DRAW);

		// enable attribute locations on shaders
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char*)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char*)FBXVertex::TexCoord1Offset);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char*)FBXVertex::NormalOffset);

		glBindVertexArray(0);
	}
}

void ShadowTutorial::UpdateFBXSceneResource(FBXFile * a_pScene)
{
	a_pScene->getRoot()->updateGlobalTransform();
}

void ShadowTutorial::RenderFBXSceneResource(FBXFile * a_pScene, glm::mat4 a_view, glm::mat4 a_projection)
{
	//Render to screen
	glUseProgram(m_shader);

	m_uTexture = glGetUniformLocation(m_shader, "myTextureSampler");
	m_uMVP = glGetUniformLocation(m_shader, "MVP");
	m_uDepthBiasMVP = glGetUniformLocation(m_shader, "DepthBiasMVP");
	m_uShadowMap = glGetUniformLocation(m_shader, "shadowMap");

	for (int i = 0; i < m_fbx->getMeshCount(); i++)
	{
		FBXMeshNode * mesh = m_fbx->getMeshByIndex(i);

		OGL_FBXRenderData *ro = (OGL_FBXRenderData*)mesh->m_userData;

		glm::mat4 MVP = (m_projectionMatrix * (glm::inverse(m_cameraMatrix)) * mesh->m_globalTransform); 
		glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
		);

		glm::mat4 depthBiasMVP = biasMatrix * m_depthMVP;
		glUniformMatrix4fv(m_uMVP, 1, false, glm::value_ptr(MVP));
		glUniformMatrix4fv(m_uDepthBiasMVP, 1, false, glm::value_ptr(depthBiasMVP));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->m_material->textures[FBXMaterial::DiffuseTexture]->handle);
		glUniform1i(m_uTexture, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_uShadowMap);
		glUniform1i(m_uShadowMap, 1);

		glBindVertexArray(ro->VAO);
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}

	// Unbind program
	//glUseProgram(0);
}

void ShadowTutorial::DestroyFBXSceneResource(FBXFile * a_pScene)
{
	// No. of Meshes and Materials in FBX File
	unsigned int meshCount = a_pScene->getMeshCount();
	unsigned int matCount = a_pScene->getMaterialCount();

	// remove meshes
	for (unsigned int i = 0; i < meshCount; i++)
	{
		// current mesh and retrieve the render data we assigned ot m_userData
		FBXMeshNode * pMesh = a_pScene->getMeshByIndex(i);
		OGL_FBXRenderData * ro = (OGL_FBXRenderData *)pMesh->m_userData;

		glDeleteBuffers(1, &ro->VBO);
		glDeleteBuffers(1, &ro->IBO);
		glDeleteVertexArrays(1, &ro->VAO);

		delete ro;
	}

	for (int i = 0; i < matCount; i++)
	{
		FBXMaterial * pMaterial = a_pScene->getMaterialByIndex(i);
		for (int j = 0; j < FBXMaterial::TextureTypes_Count; j++)
		{
			if (pMaterial->textures[j] != NULL)
			{
				glDeleteTextures(1, &pMaterial->textures[j]->handle);
			}
		}
	}
}





