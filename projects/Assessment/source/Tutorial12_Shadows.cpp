#include "Tutorial12_Shadows.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

Tutorial12_Shadows::Tutorial12_Shadows()
{

}

Tutorial12_Shadows::~Tutorial12_Shadows()
{

}

bool Tutorial12_Shadows::onCreate(int a_argc, char* a_argv[])
{
	// initialise the Gizmos helper class
	Gizmos::create();

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse(glm::lookAt(glm::vec3(10, 10, -10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));

	// get window dimensions to calculate aspect ratio
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);

	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, width / (float)height, 0.1f, 1000.0f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f, 0.25f, 0.25f, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);



	//Render to target shader.
	GLuint rtt_vs = Utility::loadShader("shadowMapRTT.vert", GL_VERTEX_SHADER);
	GLuint rtt_fs = Utility::loadShader("shadowMapRTT.frag", GL_FRAGMENT_SHADER);

	m_depthProgram = Utility::createProgram(rtt_vs, 0, 0, 0, rtt_fs);

	glDeleteShader(rtt_vs);
	glDeleteShader(rtt_fs);

	//Debugging Quad shader.
	GLuint quad_vs = Utility::loadShader("QuadShadow.vert", GL_VERTEX_SHADER);
	GLuint quad_fs = Utility::loadShader("QuadShadow.frag", GL_FRAGMENT_SHADER);

	m_2dprogram = Utility::createProgram(quad_vs, 0, 0, 0, quad_fs);

	glDeleteShader(quad_vs);
	glDeleteShader(quad_fs);

	//Standard rendering shader.
	GLuint vs = Utility::loadShader("ShadowMap.vert", GL_VERTEX_SHADER);
	GLuint fs = Utility::loadShader("ShadowMap.frag", GL_FRAGMENT_SHADER);

	m_program = Utility::createProgram(vs, 0, 0, 0, fs);

	glDeleteShader(vs);
	glDeleteShader(fs);

	createShadowBuffer();
	
	m_fbx = new FBXFile();
	if (!m_fbx->load("models/soulspear/soulspear.fbx", FBXFile::UNITS_CENTIMETER))
	{
		printf("FBX file could not be loaded!");
	}
	m_fbx->initialiseOpenGLTextures();

	InitFBXSceneResource(m_fbx);

	create2DQuad();

	return true;
}

void Tutorial12_Shadows::onUpdate(float a_deltaTime)
{
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement(m_cameraMatrix, a_deltaTime, 10);

	// clear all gizmos from last frame
	Gizmos::clear();

	// add an identity matrix gizmo
	//Gizmos::addTransform(glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1));

	// add a 20x20 grid on the XZ-plane
	for (int i = 0; i < 21; ++i)
	{
		Gizmos::addLine(glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),
			i == 10 ? glm::vec4(1, 1, 1, 1) : glm::vec4(0, 0, 0, 1));

		Gizmos::addLine(glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),
			i == 10 ? glm::vec4(1, 1, 1, 1) : glm::vec4(0, 0, 0, 1));
	}

	// quit our application when escape is pressed
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();

	UpdateFBXSceneResource(m_fbx);
}

void Tutorial12_Shadows::onDraw()
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse(m_cameraMatrix);

	// draw the gizmos from this frame
	//Gizmos::draw(m_projectionMatrix, viewMatrix);

	// get window dimensions for 2D orthographic projection
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	Gizmos::draw2D(glm::ortho<float>(0, width, 0, height, -1.0f, 1.0f));

	RenderFBXSceneResource(m_fbx, viewMatrix, m_projectionMatrix);


	

}

void Tutorial12_Shadows::onDestroy()
{
	// clean up anything we created
	Gizmos::destroy();

	// custom
	glDeleteShader(m_program);

	DestroyFBXSceneResource(m_fbx);
	m_fbx->unload();
	delete m_fbx;
	m_fbx = NULL;
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new Tutorial12_Shadows();

	if (app->create("AIE - Tutorial12_Shadows", DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT, argc, argv) == true)
		app->run();

	// explicitly control the destruction of our application
	delete app;

	return 0;
}

void Tutorial12_Shadows::InitFBXSceneResource(FBXFile * a_pScene)
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

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::TexCoord1Offset);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::NormalOffset);

		glBindVertexArray(0);
	}
}

void Tutorial12_Shadows::DestroyFBXSceneResource(FBXFile * a_pScene)
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
			// HELP
		}
	}
}

void Tutorial12_Shadows::UpdateFBXSceneResource(FBXFile * a_pScene)
{
	a_pScene->getRoot()->updateGlobalTransform();
}

void Tutorial12_Shadows::RenderFBXSceneResource(FBXFile * a_pScene, glm::mat4 a_view, glm::mat4 a_projection)
{

	//RTT
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFramebuffer);
	//glViewport(0, 0, m_shadowWidth, m_shadowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_depthProgram);
	setupLightAndShadowMatrix();
	GLuint uDepthMVP = glGetUniformLocation(m_depthProgram, "depthMVP");
	glUniformMatrix4fv(uDepthMVP, 1, false, glm::value_ptr(m_shadowProjectionViewMatrix));

	for (int i = 0; i < a_pScene->getMeshCount(); ++i)
	{
		// get current mesh
		FBXMeshNode * mesh = a_pScene->getMeshByIndex(i);

		// get render data attached to the m_userData pointer
		OGL_FBXRenderData * ro = (OGL_FBXRenderData *)mesh->m_userData;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_shadowTexture);

		glBindVertexArray(ro->VAO);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Debugging Quad
	displayShadowMap();
	

	glUseProgram(m_program);
	glm::mat4 biasMatrix(
		.02, 0.0, 0.0, 0.0,
		0.0, .02, 0.0, 0.0,
		0.0, 0.0, .02, 0.0,
		.02, .02, .02, 1.0
		);

	//vs uniforms
	GLuint uMVP = glGetUniformLocation(m_program, "MVP");
	GLuint uDepthBiasMVP = glGetUniformLocation(m_program, "DepthBiasMVP");
	
	//fs uniforms
	GLuint uTexture = glGetUniformLocation(m_program, "myTextureSampler");
	GLuint uShadowMap = glGetUniformLocation(m_program, "shadowMap");

	for (int i = 0; i < a_pScene->getMeshCount(); ++i)
	{
		// get current mesh
		FBXMeshNode * mesh = a_pScene->getMeshByIndex(i);

		// get render data attached to the m_userData pointer
		OGL_FBXRenderData * ro = (OGL_FBXRenderData *)mesh->m_userData;

		glm::mat4 MVP = m_projectionMatrix * glm::inverse(m_cameraMatrix) * mesh->m_globalTransform;
		glUniformMatrix4fv(uMVP, 1, false, glm::value_ptr(MVP));
		glm::mat4 depthBiasMVP = biasMatrix * m_shadowProjectionViewMatrix;
		glUniformMatrix4fv(uDepthBiasMVP, 1, false, glm::value_ptr(depthBiasMVP));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->m_material->textures[FBXMaterial::DiffuseTexture]->handle);
		glUniform1f(uTexture, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_shadowTexture);
		glUniform1f(uShadowMap, 1);

		glBindVertexArray(ro->VAO);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}


}

void Tutorial12_Shadows::createShadowBuffer()
{
	//resolution of the texture to put our shadow map in
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

void Tutorial12_Shadows::setupLightAndShadowMatrix()
{
	// setup light direction and shadow matrix
	glm::vec3 lightPosition = glm::vec3(5.0f, 5.0f, 1.0f);
	m_lightDirection = glm::normalize(glm::vec4(-lightPosition, 0));

	glm::mat4 depthViewMatrix = glm::lookAt(lightPosition, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-3, 3, -3, 3, -50, 50);
	m_shadowProjectionViewMatrix = depthProjectionMatrix * depthViewMatrix;
}

void Tutorial12_Shadows::create2DQuad()
{
	glGenVertexArrays(1, &m_2dQuad.vao);
	glBindVertexArray(m_2dQuad.vao);

	// create a 200x200 2D GUI quad (resize it to screen-space!)
	glm::vec2 size(400, 400);
	size.x /= DEFAULT_SCREENWIDTH;
	size.y /= DEFAULT_SCREENHEIGHT;
	size *= 2;

	// setup the quad in the top corner
	float quadVertices[] = {
		-1.0f, 1.0f - size.y, 0.0f, 1.0f, 0, 0,
		-1.0f + size.x, 1.0f - size.y, 0.0f, 1.0f, 1, 0,
		-1.0f, 1.0f, 0.0f, 1.0f, 0, 1,

		-1.0f, 1.0f, 0.0f, 1.0f, 0, 1,
		-1.0f + size.x, 1.0f - size.y, 0.0f, 1.0f, 1, 0,
		-1.0f + size.x, 1.0f, 0.0f, 1.0f, 1, 1,
	};

	glGenBuffers(1, &m_2dQuad.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_2dQuad.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 6, quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, ((char*)0) + 16);
	glBindVertexArray(0);
}

void Tutorial12_Shadows::displayShadowMap()
{
	glUseProgram(m_2dprogram);
	unsigned int texLoc = glGetUniformLocation(m_2dprogram, "shadowMap");
	glUniform1i(texLoc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_shadowTexture);
	glBindVertexArray(m_2dQuad.vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}