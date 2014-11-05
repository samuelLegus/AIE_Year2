#include "ShadowMap.h"
#include <Utilities.h>
#include "Global.h"

ShadowMap::ShadowMap()
{

}

GLuint ShadowMap::GetUniformLocation(const char * Name)
{
	return glGetUniformLocation(m_shaderProg, Name);
}

bool ShadowMap::Init()
{
	GLuint vs = Utility::loadShader("shadowMap.vert", GL_VERTEX_SHADER);
	GLuint fs = Utility::loadShader("shadowMap.frag", GL_FRAGMENT_SHADER);

	m_shaderProg = Utility::createProgram(vs, 0, 0, 0, fs);

	glDeleteShader(vs);
	glDeleteShader(fs);

	m_WVPLocation = GetUniformLocation("gWVP");
	m_textureLocation = GetUniformLocation("gShadowMap");

	if (m_WVPLocation == INVALID_UNIFORM_LOCATION ||
		m_textureLocation == INVALID_UNIFORM_LOCATION) {
		return false;
	}

	return true;
}

void ShadowMap::SetWVP(const glm::mat4& a_WVP)
{
	glUniformMatrix4fv(m_WVPLocation, 1, GL_TRUE, glm::value_ptr(a_WVP));
}

void ShadowMap::SetTextureUnit(unsigned int a_texUnit)
{
	glUniform1i(m_textureLocation, a_texUnit);
}

