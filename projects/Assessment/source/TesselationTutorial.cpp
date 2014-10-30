#include "TesselationTutorial.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

TesselationTutorial::TesselationTutorial()
{

}

TesselationTutorial::~TesselationTutorial()
{

}

void TesselationTutorial::BuildProgram()
{
	unsigned int vs = Utility::loadShader("shaders/DisplaceVS.glsl", GL_VERTEX_SHADER);
	unsigned int fs = Utility::loadShader("shaders/DisplaceFS.glsl", GL_FRAGMENT_SHADER);
	unsigned int control = Utility::loadShader("shaders/DisplaceControl.glsl", GL_TESS_CONTROL_SHADER);
	unsigned int eval = Utility::loadShader("shaders/DisplaceEval.glsl", GL_TESS_EVALUATION_SHADER);

	m_program = Utility::createProgram(vs, control, eval, 0, fs);

	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteShader(control);
	glDeleteShader(eval);

	m_displaceScale = 2;
	m_displaceLevel = 12;
}

bool TesselationTutorial::onCreate(int a_argc, char* a_argv[])
{
	Assessment::onCreate(a_argc, a_argv);

	Utility::build3DPlane(10, m_vao, m_vbo, m_ibo);

	BuildProgram();
	m_diffuseTexture.LoadFromFile("images/rock_diffuse.tga");
	m_displacementTexture.LoadFromFile( "images/rock_displacement.tga");

	m_displaceScale = 2;
	m_displaceLevel = 12;
	
	return true;
}

void TesselationTutorial::onDraw()
{
	Assessment::onDraw();

	glm::mat4 view = glm::inverse(m_cameraMatrix);
	glm::mat4 viewProj = m_projectionMatrix * view;
	glm::mat4 globalTrans = glm::mat4();

	glUseProgram(m_program);

	GLuint uVP = glGetUniformLocation(m_program, "viewProjection");
	glUniformMatrix4fv(uVP, 1, false, glm::value_ptr(viewProj));

	GLuint uGlobal = glGetUniformLocation(m_program, "global");
	glUniformMatrix4fv(uGlobal, 1, false, glm::value_ptr(globalTrans));

	// bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_diffuseTexture.textureID);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_displacementTexture.textureID);

	GLuint uTexMap = glGetUniformLocation(m_program, "textureMap");
	glUniform1i(uTexMap, 0);

	GLuint uDisplaceMap = glGetUniformLocation(m_program, "displacementMap");
	glUniform1i(uDisplaceMap, 1);

	GLuint uDisplaceScale = glGetUniformLocation(m_program, "displacementScale");
	glUniform1f(uDisplaceScale, m_displaceScale);

	GLuint uDisplaceLevel = glGetUniformLocation(m_program, "displacementLevel");
	glUniform1f(uDisplaceLevel, m_displaceLevel);

	glBindVertexArray(m_vao);
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	glDrawElements(GL_PATCHES, 6, GL_UNSIGNED_INT, nullptr);


}

void TesselationTutorial::onUpdate(float a_deltaTime)
{
	Assessment::onUpdate(a_deltaTime);

	// quit our application when escape is pressed
	if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();

	if (glfwGetKey(m_window, GLFW_KEY_I) == GLFW_PRESS)
	{

		if (m_displaceScale < 65)
		{
			m_displaceScale += 0.1;
		}
	}
	if (glfwGetKey(m_window, GLFW_KEY_K) == GLFW_PRESS)
	{
		if (m_displaceScale > 0)
		{
			m_displaceScale -= 0.1;
		}
	}

	if (glfwGetKey(m_window, GLFW_KEY_O) == GLFW_PRESS)
	{
		if (m_displaceScale < 65)
		{
			m_displaceScale += 0.1;
		}
	}
	if (glfwGetKey(m_window, GLFW_KEY_L) == GLFW_PRESS)
	{
		if (m_displaceScale > 0)
		{
			m_displaceScale -= 0.1;
		}
	}
}

void TesselationTutorial::onDestroy()
{

}