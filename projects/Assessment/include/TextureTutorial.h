#include "Assessment.h"

class TextureTutorial : public Assessment
{
private:

	Texture m_texture;
	unsigned int m_vbo;
	unsigned int m_vao;
	unsigned int m_ibo;
	unsigned int m_program;

	inline void BuildProgram()
	{
		unsigned int vs = Utility::loadShader("shaders/TextureTutorialVS.glsl", GL_VERTEX_SHADER);
		unsigned int fs = Utility::loadShader("shaders/TextureTutorialFS.glsl", GL_FRAGMENT_SHADER);
		const char* inputs[] = { "position", "colour", "textureCoordinate" };
		m_program = Utility::createProgram(vs, 0, 0, 0, fs, 3, inputs);
		glDeleteShader(vs);
		glDeleteShader(fs);
	}

	inline void BuildTexturedPlane()
	{
		Utility::build3DPlane(10, m_vao, m_vbo, m_ibo);
	}

	inline bool onCreate(int a_argc, char* a_argv[])
	{
		Assessment::onCreate(a_argc, a_argv);
		m_texture.LoadFromFile("images/lightningTexture.png");
		BuildProgram();
		BuildTexturedPlane();

		return true;
	}

	inline void onUpdate(float a_deltaTime)
	{
		Assessment::onUpdate(a_deltaTime);
	}

	inline void onDraw()
	{
		Assessment::onDraw();
		glUseProgram(m_program);

		// fetch locations of the view and projection matrices and bind them
		glm::mat4 viewMatrix = glm::inverse(m_cameraMatrix);
		unsigned int location = glGetUniformLocation(m_program, "view");
		glUniformMatrix4fv(location, 1, false, glm::value_ptr(viewMatrix));

		location = glGetUniformLocation(m_program, "projection");
		glUniformMatrix4fv(location, 1, false, glm::value_ptr(m_projectionMatrix));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture.textureID);

		location = glGetUniformLocation(m_program, "textureMap");
		glUniform1i(location, 0);

		glBindVertexArray(m_vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	}

	inline void onDestroy()
	{
		Assessment::onDestroy();
		glDeleteTextures(1, &m_texture.textureID);
		glDeleteBuffers(1, &m_vbo);
		glDeleteBuffers(1, &m_ibo);
		glDeleteVertexArrays(1, &m_vao);
	}

public:

	TextureTutorial(){}
	virtual ~TextureTutorial(){}

};
