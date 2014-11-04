#include <GL/glew.h>

#ifndef SHADOWMAPFBO_H_
#define SHADOWMAPFBO_H_

class ShadowMapFBO
{
public:
	ShadowMapFBO();
	~ShadowMapFBO();

	bool Init(unsigned int a_windowWidth, unsigned int a_windowHeight);
	void BindForWriting();
	void BindForReading(GLenum TextureUnit);

private:
	GLuint m_fbo;
	GLuint m_shadowMap;
};

#endif