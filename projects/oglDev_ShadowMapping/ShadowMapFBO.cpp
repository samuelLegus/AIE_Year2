#include "ShadowMapFBO.h"
#include <stdio.h>

ShadowMapFBO::ShadowMapFBO()
{
	// default values just because I guess
	m_fbo = 0;
	m_shadowMap = 0;
}

ShadowMapFBO::~ShadowMapFBO()
{
	if (m_fbo != 0)
		glDeleteFramebuffers(1, &m_fbo);

	if (m_shadowMap != 0)
		glDeleteTextures(1, &m_shadowMap);
}

bool ShadowMapFBO::Init(unsigned int a_windowWidth, unsigned int a_windowHeight)
{
	//create framebuffer object
	glGenFramebuffers(1, &m_fbo);

	//create the depth buffer ( the off screen texture object we are rendering the shadowmap to)
	glGenTextures(1, &m_shadowMap);
	glBindTexture(GL_TEXTURE_2D, m_shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, a_windowWidth, a_windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); 

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMap, 0);

	// Disables writes to the color buffer, cause we're only rendering to depth.
	// I assume this is to avoid unnecessary processing 
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// ERROR CHECK'N
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("FB error, status: 0x%x\n", status); 
		return false;
	}

	return true;

}

void ShadowMapFBO::BindForWriting()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void ShadowMapFBO::BindForReading(GLenum a_textureUnit)
{
	glActiveTexture(a_textureUnit);
	glBindTexture(GL_TEXTURE_2D, m_shadowMap); 
}