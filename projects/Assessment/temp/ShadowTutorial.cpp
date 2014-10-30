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

	unsigned int vert = Utility::loadShader("shaders/TextureTutorialVS.glsl", GL_VERTEX_SHADER);
	unsigned int frag = Utility::loadShader("shaders/TextureTutorialFS.glsl", GL_FRAGMENT_SHADER);
	const char* inputs[] = { "position", "colour", "textureCoordinate" };
	m_planeShader = Utility::createProgram(vert, 0, 0, 0, frag, 3, inputs);
	glDeleteShader(vert);
	glDeleteShader(frag);
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
	glGenTextures(1, &m_shadowTexture);
	glBindTexture(GL_TEXTURE_2D, m_shadowTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, m_shadowWidth, m_shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_shadowTexture, 0);

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
	m_shadowProjectionViewMatrix = depthProjectionMatrix * depthViewMatrix;
}


bool ShadowTutorial::onCreate(int a_argc, char * a_argv[])
{
	Assessment::onCreate(a_argc, a_argv);

	BuildProgram(); 
	BuildDepthProgram();
	CreateShadowBuffer();

	Utility::build3DPlane(10, m_planeVAO, m_planeVBO, m_planeIBO);

	m_fbx = new FBXFile();
	if (!m_fbx->load("models/soulspear/soulspear.fbx", FBXFile::UNITS_CENTIMETER))
	{
		printf("FBX file could not be loaded!");
	}
	m_fbx->initialiseOpenGLTextures();
	InitFBXSceneResource(m_fbx);

	m_planeTexture.LoadFromFile("images/decayTexture.png");

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
	glm::mat4 view = glm::inverse(m_cameraMatrix);
	

	glUseProgram(m_planeShader);

	// fetch locations of the view and projection matrices and bind them
	glm::mat4 viewMatrix = glm::inverse(m_cameraMatrix);
	unsigned int location = glGetUniformLocation(m_planeShader, "view");
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(viewMatrix));

	location = glGetUniformLocation(m_planeShader, "projection");
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(m_projectionMatrix));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_planeTexture.textureID);

	location = glGetUniformLocation(m_planeShader, "textureMap");
	glUniform1i(location, 0);

	glBindVertexArray(m_planeVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	//Shadow map stuff here ???
	glUseProgram(m_depthShader);
	CalculateLightAndShadowMatrix();
	GLuint uShadowMVP = glGetUniformLocation(m_depthShader, "lightProjectionViewWorld");
	glUniformMatrix4fv(uShadowMVP, 1, false, glm::value_ptr(m_shadowProjectionViewMatrix));

	RenderFBXSceneResource(m_fbx, view, m_projectionMatrix);

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

	glUseProgram(m_shader);

	GLuint uDiffuseTex = glGetUniformLocation(m_shader, "DiffuseTexture");
	//GLuint uMVP = glGetUniformLocation(m_shader, "MVP");

	GLuint uModel = glGetUniformLocation(m_shader, "Model");
	GLuint uView = glGetUniformLocation(m_shader, "View");
	GLuint uProjection = glGetUniformLocation(m_shader, "Projection");

	for (int i = 0; i < m_fbx->getMeshCount(); i++)
	{
		FBXMeshNode * mesh = m_fbx->getMeshByIndex(i);

		OGL_FBXRenderData *ro = (OGL_FBXRenderData*)mesh->m_userData;

		glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(mesh->m_globalTransform));
		glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(a_view));
		glUniformMatrix4fv(uProjection, 1, GL_FALSE, glm::value_ptr(a_projection));

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mesh->m_material->textures[FBXMaterial::DiffuseTexture]->handle);
		glUniform1i(uDiffuseTex, 1);

		glBindVertexArray(ro->VAO);
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}

	// Unbind program
	glUseProgram(0);
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





