#include "Assessment.h"

class ShadowMappingTutorial : public Assessment
{
private:

	GLuint m_shader;
	GLuint m_2dprogram;
	GLuint m_depthShader;
	
	GLuint m_fbo;
	GLuint m_depthTexture;

	struct quad
	{
	
		GLuint vao;
		GLuint vbo;
	} m_2dQuad;

	inline void create2DQuad()
	{
		glGenVertexArrays(1, &m_2dQuad.vao);
		glBindVertexArray(m_2dQuad.vao);

		// create a 200x200 2D GUI quad (resize it to screen-space!)
		glm::vec2 size(200, 200);
		size.x /= 1024;
		size.y /= 768;
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 6 * 6, quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, ((char*)0) + 16);
		glBindVertexArray(0);
	}

	inline void displayShadowMap()
	{
		glUseProgram(m_2dprogram);
		unsigned int texLoc = glGetUniformLocation(m_2dprogram, "shadowMap");
		glUniform1i(texLoc, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_depthTexture);
		glBindVertexArray(m_2dQuad.vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	//uniforms

	GLuint m_uDepthMVP;
	GLuint m_uTexture;
	GLuint m_uMVP;
	GLuint m_uDepthBiasMVP;
	GLuint m_uShadowMap;

	
	//the model
	FBXFile * m_fbx; //aka Texture, VBO, IBO, VAO in here as well.

	bool onCreate(int a_argc, char* a_argv[]);

	void onUpdate(float a_deltaTime);

	void onDraw();

	void onDestroy();

public:

	ShadowMappingTutorial();

	virtual ~ShadowMappingTutorial();

	void InitFBXSceneResource(FBXFile *a_pScene);

	void DestroyFBXSceneResource(FBXFile *a_pScene);

	void UpdateFBXSceneResource(FBXFile *a_pScene);

	void RenderFBXSceneResource(FBXFile *a_pScene, glm::mat4 a_view, glm::mat4 a_projection);

};