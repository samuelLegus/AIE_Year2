#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class ShadowMap
{
private:

	GLuint m_shaderProg;
	GLuint m_WVPLocation;
	GLuint m_textureLocation;

public:
	ShadowMap();
	bool Init();

	void SetWVP(const glm::mat4& a_WVP);
	void SetTextureUnit(unsigned int TextureUnit);

	GLuint GetUniformLocation(const char * Name); 

};